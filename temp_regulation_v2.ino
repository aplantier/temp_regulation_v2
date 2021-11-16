#include "OneWire.h"
#include "DallasTemperature.h"
#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
// GLOBLA VARIABLES FOR THE SYSTEM 

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer
unsigned long loop_period = 10L * 1000; /* =>  10000ms : 10 s */

// :: BRANCHING OF THE PIN ::
// -- output
int pinHeater         = 21;
int pinClim           = 19;
int pinVentilo        = 18;

// -- input 
int pinPhoto         = A5;
int pinTemperature   = 23;


// intit of the temperature object 
OneWire oneWire(pinTemperature); 
DallasTemperature tempSensor(&oneWire);


//GPIO CANAL 
int gpioVentilo     = 0;

// DEFINITIONS OF RANGE 
float SHJ            = 28.4; // Seuil Haut jour 
float SBJ            = 27.5;//  Seuil Bas jour 

float SHN            = 30.0; // Seuil Haut Nuit 
float SBN            = 26.5; // Seuil Bas nuit 

float HIGHVENTIL     = 2; // Delta to activate the High Ventilation mode 
int SJN              = 100;// Seuil de detection jour nuit pour la luminosité             
int MODE             = 1; // mode 0: manual 1:autoregul

// Definition of unit 

bool regulation=true; 


struct measurement {

   int mode_;//0: force, 1:regulation 
   // Photo
   int photo_value;
   int photo_tresh;
   bool day_;
   
   // temperature
   float tem_value;
   //> clim 
   float tem_tresh_high_night;
   float tem_tresh_high_day;
   float tem_tresh_veryhigh;
   short clim_;// percentage rate of rotation 
   //> heater
   float tem_tresh_low_night;
   float tem_tresh_low_day;
   short heat_;
}; 



#include "configuration.h"
#include "modes.h"
#include "wifi-config.h" 
#include "http-command.h"



// Cration of the Web server 
  // WiFiServer server(80);
  AsyncWebServer server(80);


void setup() {
   Serial.begin(9600);// init bode 
 //  Serial.println(" :: DEVICE INITIALISATION ::");
   
   delay(2*1000);// Delay on startup 
   
   if( !SPIFFS.begin(true)){
      Serial.println("Opening SPIFFS partition has failed \n");
      return;
   }

   // CONFIG WIFI   
   while(!Serial);// Wait Til serial conection
   connect_wifi();// connect on a network hard registered in the wifi-config.h file. 

   print_network_status();// print the selected conection if found 
   server.begin();
  
   // SENSOR CONFIG 
   // Starting the temperature module 
   tempSensor.begin();

   // :: CONFIGURATION OF PIN :: 

   // -- output 
   // Initialisation of  output pin 19 21 for the leds 
   pinMode(pinClim, OUTPUT);
   pinMode(pinHeater, OUTPUT);
   ledcAttachPin(pinVentilo,gpioVentilo); // attach the gpio canal to the output pin 
   ledcSetup(gpioVentilo,25000,8);

};

void loop() {
   
    WiFiClient client = server.available();
  
  if (client) { // incoming client
    Serial.println("New client is connecting !");
    // an http request ends with a blank line CRLF
    
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {
	
        char c = client.read(); // Echo on the console
        Serial.write(c);
	
        // if you've gotten to a CRLF the http GET request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) { 
          //httpReply(client);
          postHTMLpage(client);
          break;
        }
        if (c == '\r') { // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') { // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    
    // give the web browser time to receive the data
    delay(loop_period); // ms
    
    // close the connection :
    client.stop();
    Serial.println("client disconnected");
  }



   measurement curMes; 
   // VARIABLES 
   String command =""; 
   while(Serial.available()> 0){
      command = Serial.readStringUntil('\n');
      doComand(command,&curMes);
   }
   
  String url = String(host)+path+params;

  //Send an HTTP request every loop_period in ms
 /*
  if ((millis() - lastTime) > loop_period) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      String ret = httpGETRequest(url.c_str());
      Serial.println(ret);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  */

  curMes.mode_=MODE; 
  // save photo value
  curMes.photo_value = analogRead(pinPhoto);
   // Save temp value 
  tempSensor.requestTemperaturesByIndex(0);
  delay(500); 
  curMes.tem_value =tempSensor.getTempCByIndex(0);

   // Save threhold 
  // Value of the threshold can have been changed, we must reloqd them
  curMes.photo_tresh=SJN;
  curMes.tem_tresh_low_day=SBJ;
  curMes.tem_tresh_high_day=SHJ;
  curMes.tem_tresh_low_night=SBN;
  curMes.tem_tresh_high_night=SHN;
  curMes.tem_tresh_veryhigh=HIGHVENTIL;
  switch(curMes.mode_){
     case 1:
         doRegulation(&curMes);
        break;
     case 0:

        break;
  }
  applyConfiguration(curMes);
  String json_result=buildMeasure_JSON(curMes);
  Serial.println(json_result);
  //Serial.print("Analog read : ");
  //Serial.println(photoValue, DEC);
 
  delay(1000); // delay 1s 

}

