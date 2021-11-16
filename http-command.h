#ifndef HTTP_CLIENT__H
#define HTTP_CLIENT__H
#include <WiFi.h>
#include <HTTPClient.h>

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
#endif 
