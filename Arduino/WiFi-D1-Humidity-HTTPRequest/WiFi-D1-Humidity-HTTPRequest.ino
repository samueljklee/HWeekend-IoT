#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "robot";
const char* password = "";

#define DHTPIN D6
#define DHTTYPE DHT11 
#define ledPin D2
 
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80); 

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting");
  delay(3000);
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}
 
void loop() 
{   
  // Check if a client has connected
  WiFiClient client = server.available();
 
  // Wait until the client sends some data
  Serial.println("new client");
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    Serial.println("LED ON");
    digitalWrite(ledPin, HIGH);
    digitalWrite(2, HIGH);
    value = HIGH;
    ledFunc(client);
  } 
  if (request.indexOf("/LED=OFF") != -1){
    Serial.println("LED ON");
    digitalWrite(ledPin, LOW);
    digitalWrite(2, HIGH);
    value = LOW;
    ledFunc(client);
  }
   if (request.indexOf("/Humidity") != -1){
      humidityFunc(client);
  }
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}

void ledFunc(WiFiClient client){
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>");
  client.println("</html>");
  
}

void humidityFunc(WiFiClient client){
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float heatIndex = dht.computeHeatIndex(temperature,humidity,false);
  
  if (isnan(humidity) || isnan(temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<br><br>");
  
  client.print("Humidity: ");
  client.print(humidity);
  client.print(" Temperature: ");
  client.print(temperature);
  client.print(" Heat Index: ");
  client.println(heatIndex);   
 
  client.println("</html>");

  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    Serial.println("WIFI CONNECTED GOING TO HTTP FUNCTION");
    httpRequestFunc();
 }else{
    Serial.println("Error in WiFi connection");   
 }
}

void httpRequestFunc(){
  HTTPClient http;   
  http.begin("http://flip3.engr.oregonstate.edu:3000/hello");     
  http.addHeader("Content-Type", "text/plain"); 
  
  int httpCode = http.POST("Message from ESP8266"); 
  String payload = http.getString();           
        
  Serial.println(httpCode); 
  Serial.println(payload);  
  http.end();
}

