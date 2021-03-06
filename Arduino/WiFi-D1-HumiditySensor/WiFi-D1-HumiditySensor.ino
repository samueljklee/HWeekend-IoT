#include <DHT.h>
#include <ESP8266WiFi.h>
 
#define DHTPIN D6
#define DHTTYPE DHT11 
#define ledPin D2

const char* ssid = "robot";
const char* password = "";
 
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
  WiFiClient client = server.available();
  Serial.println("new client");
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
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
}

