#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
 
void setup() {
  Serial.begin(115200);
  WiFi.begin("robot", ""); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for connection");
  }
}
 
void loop() {
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin("http://flip3.engr.oregonstate.edu:3000/hello");      //Specify request destination
   http.addHeader("Content-Type", "text/plain");  //Specify content-type header
   int httpCode = http.POST("Message from ESP8266");   //Send the request
   String payload = http.getString();                  //Get the response payload
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
 }
}
