#ifndef HTTP_CLIENT__H
#define HTTP_CLIENT__H
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Ping.h> //use of the ping api to test if the remote device is reachable 
#include <ArduinoJson.h> // Using the JSON Api 
/*
 *
 *
 * file : http-command.h
 * PLANTIER Antoine 
 * This file describe the http command
 */



// ENVIRONEMENT VARIABLES 
char host[100] = "http://httpbin.org"; // = "http://worldtimeapi.org/";
const int httpPort = 80;
String path = "/get";
String params = "?led1=""OFF""&led2=""ON""";

String httpGETRequest(const char* UrlServer) {
  // return the response of the GET request to UrlServer
  HTTPClient http; // http protocol entity =>  client

  Serial.print("Requesting URL : ");
  Serial.println(UrlServer);

  // Your IP address with path or Domain name with URL path
  http.begin(UrlServer); // Parse URL of the server

  // Send HTTP request
  int httpResponseCode = http.GET();

  // Get the response => will fill payload String
  String payload = "{}";
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code on HTTP GET Request :");
    Serial.println(httpResponseCode);
  }
  // End connection and Free resources
  http.end();

  return payload;
}
/*--------------------------------*/
void httpReply(WiFiClient client) {
  // this method makes a simple HTTP GET reply
  // the body syntax is HTML
  // => supposed to be displayed by a navigator
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after
                                        //completion of the response
  client.println("Refresh: 5");         // refresh the page automatically every 5 sec

  client.println(); // Empty line between header and body

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("Hello, je tourne depuis : "); // Returns the ms passed since the ESP
                                              // began running the current program.
  client.print(millis()/1000); // On pourrait sans doute donner une info
  client.println("s <br />");  // plus pertinente ? temperature ?
  client.println("</html>");
}

void postHTMLpage(WiFiClient client) {

   Serial.println("SPIFFS FUNCTION");
   client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after
                                        //completion of the response
  client.println("Refresh: 5");         // refresh the page automatically every 5 sec

  client.println(); // Empty line between header and body


   File page  = SPIFFS.open("/statut.html","r");
   while(page.available()){
//      Serial.println(page.read());
      client.println(page.read());

   }
   page.close();
}
 String processor(const String& var){
  Serial.println(var);
  if(var == "UPTIME"){
     return String(millis()/1000)  ;
  }
  if(var == "WHERE"){
     return " some where ..."  ;
  }
  if(var == "SSID"){
     return curMes.ssid  ;
  }
  if(var == "MAC"){
     return curMes.mac  ;
  }
  if(var == "IP"){
     return curMes.ip  ;
  }
 if(var == "WHERE"){
     return " some where ..."  ;
  }
  if(var == "TEMPERATURE"){
     return String(curMes.tem_value) ;
  }
  if(var == "LIGHT"){
     return String(curMes.photo_value)  ;
  }
  if(var == "COOLER"){
     String cooler_str ="OFF" ; 
     if(curMes.clim_!=0)
     {
         cooler_str = "ON ( Fan speed : ";
         cooler_str+= String(curMes.clim_);
         cooler_str+=")"; 
     }
        
     return cooler_str  ;
  }
  if(var == "HEATER"){
     if(curMes.heat_>0)
        return " ON "  ;
     else
        return " OFF "; 
  }
  if(var == "LT"){
     return String(curMes.photo_tresh)  ;
  }
  if(var == "SBJ"){
     return String(curMes.tem_tresh_low_day);
  }
  if(var == "SHJ"){
     return String(curMes.tem_tresh_high_day);
  }
  if(var == "SBN"){
     return String(curMes.tem_tresh_low_night);
  }
  if(var == "SHJ"){
     return String(curMes.tem_tresh_high_night);
  }

  if(var == "PRT_IP"){
     return String(curMes.report_ip);
  }
  if(var == "PRT_PORT"){
     return String(curMes.report_port);
  }
  if(var == "PRT_T"){
     return String(curMes.report_delay);
  }


 return String();
}

void configRoute( AsyncWebServer * server){
   server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      if(curMes.mode_ == 1 ) 
         request->send(SPIFFS, "/statut.html", String(), false, processor);
      else
         request->send(SPIFFS, "/control.html", String(), false, processor);

   });
   server->on("/mode", HTTP_GET , [](AsyncWebServerRequest *request){
   String input; 
         if(request->hasParam("mode")){
            input = request->getParam("mode")->value();
            MODE = input.toInt();
         } 
         MODE = curMes.mode_==1?0:1;
         request->redirect("/");
         
   });

   // Response for the glyph request ( update of the temp value in the html page ) 
server->on("/light", HTTP_GET , [](AsyncWebServerRequest *request){
        request->send(200, "text/html", String(curMes.photo_value));
      });
server->on("/temperature", HTTP_GET , [](AsyncWebServerRequest *request){
        request->send(200, "text/html", String(curMes.tem_value));
      });

/* Response for the /set 
 * The /set allow to change the value of the parameters in the esp32 to change the value of the parameters in the esp32  
 */
server->on("/set", HTTP_GET , [](AsyncWebServerRequest *request){
   String input_value  = "" ;    
      if(request->hasParam("sbn")){
         input_value = request->getParam("sbn")->value();
         SBN = input_value.toFloat();  
      }
   if(request->hasParam("sbj")){
         input_value = request->getParam("sbj")->value();
         SBJ = input_value.toFloat();  
      }
   if(request->hasParam("shj")){
         input_value = request->getParam("shj")->value();
         SHJ = input_value.toFloat();  
      }

   if(request->hasParam("shn")){
         input_value = request->getParam("shn")->value();
         SHN = input_value.toFloat();  
      }

   if(request->hasParam("sjn")){
         input_value = request->getParam("shj")->value();
         SJN = input_value.toFloat();  
      }
   if(request->hasParam("sp")){
         input_value = request->getParam("sp")->value();
         DELAY = input_value.toInt();  
      }


      }); 
/* Response for the /value 
 * The /value allow request for value of esp32 configuration.
 * It return a json object containing the differents values
 */
server->on("/value", HTTP_GET , [](AsyncWebServerRequest *request){
  StaticJsonDocument<2400> returnValue; 

   if(request->hasParam("temperature")){
     returnValue["temperature"] = String(curMes.tem_value);
   }
   
   if(request->hasParam("light")){
     returnValue["light"] = String(curMes.photo_value);

   }
   if(request->hasParam("cooler")){
     returnValue["cooler"] = String(curMes.clim_);

   }
   if(request->hasParam("heater")){
     returnValue["heater"] = String(curMes.heat_);

   }
   if(request->hasParam("ip")){
     returnValue["ip"] = curMes.report_ip;

   }
   if(request->hasParam("port")){
     returnValue["port"] = String(curMes.report_port);

   }
   if(request->hasParam("sp")){
     returnValue["sp"] = String(curMes.report_delay);

   }
   if(request->hasParam("ligh_treshold")){
     returnValue["light_treshold"] = String(curMes.photo_tresh);

   }
   if(request->hasParam("sbn")){
     returnValue["sbn"] = String(curMes.tem_tresh_low_night);

   }
   if(request->hasParam("shn")){
     returnValue["shn"] = String(curMes.tem_tresh_high_night);

   }
   if(request->hasParam("sbj")){
     returnValue["sbj"] = String(curMes.tem_tresh_low_day);

   }
   if(request->hasParam("shj")){
     returnValue["shj"] = String(curMes.tem_tresh_high_day);

   }
   if(request->hasParam("uptime")){
     returnValue["uptime"] = "hello";

   }
   if(request->hasParam("ssid")){
     returnValue["ssid"] = String(curMes.ssid);

   }
   if(request->hasParam("mac")){
     returnValue["mac"] = String(curMes.mac);

   }
   if(request->hasParam("ip_esp")){
     returnValue["ip_esp"] = String(curMes.ip);
   }
  
   char buffer [1024];
   serializeJson(returnValue, buffer);
   request->send(200, "appliclation/json", buffer);
      });


// Comand to pass in manual mode and set the new configuration 
server->on("/command", HTTP_GET , [](AsyncWebServerRequest *request){
   String input; 
   if(MODE==1)
      return;
   else{
      if(request->hasParam("heater")){
         input = request->getParam("heater")->value();
         curMes.heat_ = input.toInt();
      }   
      if(request->hasParam("cooler")){
         input = request->getParam("cooler")->value();
         curMes.clim_= input.toInt();
      }   
      if(curMes.clim_!=0 && request->hasParam("fan-speed")){// By default if clim is disabled, not taking the value

         input = request->getParam("fan-speed")->value();
         curMes.clim_= input.toInt();
      }
        request->send(200, "text/html", "New configuration saved<br><a href=\"/\">Return to Home Page</a>");
   }


      });
/**
 * This acessor is for the periodic report. When the congiguration is send on /target, the 
 * configuration is set and the host is tested. If the client respond, the periodic report is on 
 */
server->on("/target", HTTP_POST , [](AsyncWebServerRequest *request){
    String ip =""; 
    int port =0 ;
    int delay=0;
    char buff[50]; 

   if(request->hasParam("ip",true)){// on POST, must specify option to not test page 
          ip = request->getParam("ip",true)->value();
   }
   if(request->hasParam("port",true)){
         port= request->getParam("port",true)->value().toInt();
   }
   if(request->hasParam("sp",true)){
          delay= request->getParam("sp",true)->value().toInt();
   }

   ip.toCharArray(buff,50);
   bool pingSuccess = Ping.ping(buff,3);
   if(pingSuccess){
        request->send(200, "text/html", "The host is on , New configuration saved<br><a href=\"/\">Return to Home Page</a>");
          curMes.report_ip = request->getParam("ip",true)->value();
          curMes.report_port = request->getParam("port",true)->value().toInt();
          //curMes.report_delay = request->getParam("sp",true)->value().toInt();
          DELAY=curMes.report_delay = request->getParam("sp",true)->value().toInt();
          curMes.report_on=true;
         char port_buff[16]; 
            itoa(port, port_buff, 10);
         strcpy(curMes.report_server,"http://");
         strcat(curMes.report_server,buff);
         strcat(curMes.report_server,":");
         strcat(curMes.report_server,port_buff);
         strcat(curMes.report_server,"/result");
         Serial.println(curMes.report_server);

   }
        else{
          request->send(200, "text/html", "The host is offline  <br><a href=\"/\">Return to Home Page</a>");
          curMes.report_delay=0;
          curMes.report_on=false;
        }
});



}





#endif 
