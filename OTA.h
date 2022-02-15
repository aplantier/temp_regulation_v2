/**
 * OTA.h 
 * Based on the TP1. configuration 
 * Adapted to the TP2 project 
 */
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>

unsigned long previousMillis = 0 ; 
unsigned long otamillis; 

void setupOTA( const char * nameprefix ) {
   const int maxlen = 40 ; 
   char fullhostname [maxlen];
   uint8_t mac[6]; 
   WiFi.macAddress(mac);
   
   ArduinoOTA.setPort(1200); 
   ArduinoOTA.setHostname("esp32"); // naming the device 

   snprintf(fullhostname, maxlen, " %s-%02x%02x%02x", nameprefix, mac[3], mac[4], mac[5]);
   ArduinoOTA.setHostname(fullhostname);

   ArduinoOTA
   .onStart([]() {
      String type; 
      if(ArduinoOTA.getCommand() == U_FLASH )
         type = "sketch";
      else
         type = "filesystem";
      
   SPIFFS.end();
   Serial.println("Start Updating" + type);
   otamillis = millis(); 

   })
   .onEnd([] () {
      Serial.println("\nEND");
      Serial.print((millis()- otamillis)/1000.0);
      Serial.println("second");

      })
   .onProgress([](unsigned int progress, unsigned int total ){
      Serial.printf("Progress : %u%%\n", (progress/(total/100))); 
      })
   .onError( [] (ota_error_t error){
      Serial.printf("Error[%u]:",error);
switch ( error ) {
// e r r e u r d ’ a u t h e n t i f i c a t i o n , mauvais mot de p a s s e OTA
case OTA_AUTH_ERROR:
 Serial.println("OTA_AUTH_ERROR: ␣Auth␣ F a i l e d " ) ;
break ;
// e r r e u r l o r s du dÃ©marrage de l a MaJ ( f l a s h i n s u f f i s a n t e )
case OTA_BEGIN_ERROR:
 Serial.println( "OTA_BEGIN_ERROR: ␣ Begin ␣ F a i l e d " ) ;
break ;
// i m p o s s i b l e de s e c o n n e c t e r Ã l ’ IDE Arduino
case OTA_CONNECT_ERROR:
 Serial.println ( "OTA_CONNECT_ERROR: ␣ Connect ␣ F a i l e d " ) ;
break ;
// E r r e u r de r Ã © c e p t i o n d e s donnÃ©es
case OTA_RECEIVE_ERROR:
 Serial.println ( "OTA_RECEIVE_ERROR: ␣ R e c e i v e ␣ F a i l e d " ) ;
break ;
// E r r e u r l o r s de l a c o n f i r m a t i o n de MaJ
case OTA_END_ERROR:
 Serial.println ( "OTA_END_ERROR: ␣ R e c e i v e ␣ F a i l e d " ) ;
break ;
// E r r e u r i n c o n n u e
default :
 Serial.println ( " Erreur ␣ inconnue " ) ;
}

         });
ArduinoOTA.begin();
Serial.println("OTA INITIALIZED");
Serial.println("IP ADDR : ");
Serial.println(WiFi.localIP());
   
}
