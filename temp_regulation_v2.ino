#include "OneWire.h"
#include "DallasTemperature.h"
#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "OTA.h"
#include <Wire.h>
#include <PubSubClient.h>

// GLOBLA VARIABLES FOR THE SYSTEM 

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer
unsigned long loop_period = 10L * 1000; /* =>  10000ms : 10 s */

// :: BRANCHING OF THE PIN ::
// -- output
#define LED_BUILTIN 2
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
int MODE             = 1; // mode 0: manual 1:autoregul 2: fire Alert 

int DELAY            = 0;
// Definition of unit 
#define BATIMENT "bat1"
#define ETAGE "science"
#define SALLE "iot"


bool internalLedState=false;
bool regulation=true; 
unsigned long elapsed_time = 0; // this i sued to send periodicaly the report 

struct measurement {
   // Modules info 
   // TODO add this in the json report
    
   String ssid; 
   String mac; 
   String ip;
   char batiment[30];
   char etage[3];
   char salle[30];
   char fullTopic[128];
   char commandTopic[150];
   char batTopic[32];

   String report_ip;// This is the  addr to send periodically the sensor values 
   int report_port;
   int report_delay;
   bool report_on;
   char report_server[100];

   int mode_;//0: force, 1:regulation , 2 alerte Incendie 
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


measurement curMes; 

WiFiClient client; 
// MQTT Broker 
//const char * MQTT_SERVER_ADDR = "192.168.1.13"; 
const char * MQTT_SERVER_ADDR = "test.mosquitto.org"; 
//const char * MQTT_SERVER_ADDR = "192.168.1.13"; 
//const char * MQTT_SERVER_ADDR = "192.168.1.13"; 
const int MQTT_SERVER_PORT = 1883; 
int MQTT_ACK_PERIOD = 3; 
int mqtt_cycle_counter =0; 
// MQTT Topic 
#define TOPIC_TEMP "/sensor/temp"
#define TOPIC_LUM "/sensor/light"
#define ACK_TOPIC "/ack"
#define FIRE_SUSPECT_TOPIC "/firesuspect"
//MQTT Client
  PubSubClient mqtt_client(client);

// FIRE MODE 

  int fireCode = 0 ; // 0: ras 1: fire suspected, 2:fire
  bool fireInBuilding=false; 
#define FIRE_MESURE_NUMBER 3 
#define FIRE_ALERT_DELAY 5
  int fire_alert_counter = 0; // this counter count the cycles in fireCode state . passed the FIRE_ALERT_DELAY value, we reset the fireCode to 0 , 
  float prevTemp[FIRE_MESURE_NUMBER]={0,0,0};
  short fire_cycle_counter = 0 ;
#include "configuration.h"
#include "modes.h"
#include "wifi-config.h" 
#include "http-command.h"
#include "mqtt-command.h"


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
   connect_wifi(&curMes);// connect on a network hard registered in the wifi-config.h file. 

   delay(5000);
   setupOTA("ESP OTA ON" );// Enable the OTA dowloading with the Arduino IDE 
   print_network_status();// print the selected conection if found 
   server.begin();
  
   mqtt_client.setServer(MQTT_SERVER_ADDR,MQTT_SERVER_PORT);
   mqtt_client.setCallback(mqtt_pubcallback);
   // SENSOR CONFIG 
   // Starting the temperature module 
   tempSensor.begin();

   // :: CONFIGURATION OF PIN :: 

   // -- output
   // Initialisation of  output pin 19 21 for the leds 
   pinMode(pinClim, OUTPUT);
   pinMode(pinHeater, OUTPUT);
   pinMode(LED_BUILTIN,OUTPUT);
   digitalWrite(LED_BUILTIN,LOW);
   ledcAttachPin(pinVentilo,gpioVentilo); // attach the gpio canal to the output pin 
   ledcSetup(gpioVentilo,25000,8);

   configRoute( & server) ; 
   curMes.report_port=0;
   curMes.report_ip="0.0.0.0";// default ip, not used replaced by the ip setted up by the user in the web page 
   curMes.report_delay=0;
   curMes.report_on=false;
   // CONFIGURATION OF THE ROUTES 
   // CONFIGURATION OF THE LOCALISATION of the esp & the full personal topic of the esp 
   // INDIVIDUAL TOPIC 
   strcpy(curMes.batiment,BATIMENT);
   strcpy(curMes.salle,SALLE);
   strcpy(curMes.etage,ETAGE);
  
   strcat(curMes.fullTopic,"/");
   strcat(curMes.fullTopic,BATIMENT);
   strcat(curMes.fullTopic,"/");
   strcat(curMes.fullTopic,ETAGE);
   strcat(curMes.fullTopic,"/");
   strcat(curMes.fullTopic,SALLE);
   strcat(curMes.fullTopic,"/");
   
   char buffMac[30]; 
   curMes.mac.toCharArray(buffMac,curMes.mac.length()+1);// +1 for the last string char 
   strcat(curMes.fullTopic,buffMac);
  
   //individual command topic 
   strcpy(curMes.commandTopic,curMes.fullTopic);
   strcat(curMes.commandTopic,"/command");

   //BAT TOPIC 
   strcpy(curMes.batTopic,"/");
   strcat(curMes.batTopic,BATIMENT);
}

void loop() {
   // Handle OTA 
   ArduinoOTA.handle();
   // VARIABLES 
   
   if(!mqtt_client.connected()) {// SUBSCRIBED TOPICS 

     char * topics []={
        curMes.commandTopic,// the individual command  topic 
        curMes.batTopic// the building topic 
     };
   mqtt_mysubscribe (topics,2) ;// Register to own topic 
   }

  String url = String(host)+path+params;

  // save photo value
  curMes.photo_value = analogRead(pinPhoto);
   // Save temp value 
  tempSensor.requestTemperaturesByIndex(0);
  delay(500); 
  curMes.tem_value =tempSensor.getTempCByIndex(0);
   
  //+++++++++++++++++++++++++ TP4 
publishtemperatureMQTT(curMes,TOPIC_TEMP);
 publishLightMQTT(curMes,TOPIC_LUM);
  
  if( fireCode ==1/* could be a fire */ ){// since the fire detect has been detected we stay in that state 
                     // while the fire is not confirmed by other esp OR the fire is comfirmed by measure reaching a limit.
     if ( fireInBuilding || detectFire(curMes.tem_value, prevTemp,FIRE_MESURE_NUMBER,0.5,28 ) == 2)
      {
         fireCode=2;// fire is detected 
      }

      if(fireCode != 2 && fire_alert_counter++>=FIRE_ALERT_DELAY){// after a period without confirmation of fire 
         fireCode =0 ; 
         MODE=1; 
         
         fire_alert_counter=0;
      }
   }else
   {
     fireCode = detectFire(curMes.tem_value, prevTemp,FIRE_MESURE_NUMBER,0.5,28 );
   }
  switch(fireCode){
     case 0: 
         // no changing mode 
        Serial.println(" No diff ");
        break; 
     case 1:
        Serial.println(" Fire suspected ");
         MODE=2; 
         break;
     case 2:
         MODE=3; 
        Serial.println(" Fire");
         break;
  }

  curMes.mode_=MODE; 
   // Save threhold 
  // Value of the threshold can have been changed, we must reloqd them
  curMes.photo_tresh=SJN;
  curMes.tem_tresh_low_day=SBJ;
  curMes.tem_tresh_high_day=SHJ;
  curMes.tem_tresh_low_night=SBN;
  curMes.tem_tresh_high_night=SHN;
  curMes.tem_tresh_veryhigh=HIGHVENTIL;
  curMes.report_delay= DELAY; 
  //Serial.print(DELAY);
  switch(curMes.mode_){
     case 1:
         doRegulation(&curMes);
        break;
     case 0: // manual mode 
        break;
     case 2:// fire suspected 
      publishstatusMQTT(curMes,curMes.batTopic); 

        break;
     case 3://fire
        Serial.println("FIRE MODE "); 
        blinkInternalLed();
        break;
      default:// fire 
         blinkInternalLed();

  }
  applyConfiguration(curMes);
  String json_result=buildMeasure_JSON(curMes);
  

  if(curMes.report_on && millis() > (elapsed_time+curMes.report_delay*1000)){
     elapsed_time=millis();
     WiFiClient client; 
     HTTPClient http;

     char server[100]; 
     http.begin(client,curMes.report_server);
     http.addHeader("Content-Type", "application/json");
     int httpResponseCode = http.POST(json_result);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
     http.end();    

  }

if( curMes.mode_==3 || mqtt_cycle_counter++ == MQTT_ACK_PERIOD){// Sending acknowledgment on personal topic bteach MQTT_ACK_PERIOD
   publishstatusMQTT(curMes,curMes.fullTopic); 
   mqtt_cycle_counter=0;
}

  delay(1000); // delay 1s 
  mqtt_client.loop();

}

