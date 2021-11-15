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
#endif 
