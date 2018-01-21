#include <math.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "robot";
const char* password = "";

WiFiServer server(80);

//intruder
#define LASER D7
#define analogPin A0
#define RLED D4
#define GLED D5
#define BLED D6

//temperature
#define ONEWIRE D3
OneWire oneWire(ONEWIRE);
DallasTemperature sensor(&oneWire);


int delayTime = 1000;
bool intruder = false;

void setup() { 
  Serial.begin(9600); 
  pinMode(LASER, OUTPUT); 
  pinMode(RLED, OUTPUT); 
  pinMode(GLED, OUTPUT); 
  pinMode(BLED, OUTPUT);
  sensor.begin();

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  startLaser();
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  if(request.indexOf("/home") != -1){
    showHome(client);
  }
  if (request.indexOf("/all") != -1) {
    getAllData(client);
  }
  if (request.indexOf("/checkTemperature") != -1) {
    checkTemperature(client);
  }  
  checkIntruderGarden();
  if (request.indexOf("/checkIntruderGarden") != -1){
    checkIntruderGarden(client);
  }
  
}

void checkTemperature(WiFiClient client){
  sensor.requestTemperatures(); //Request temperatures from the sensor
  
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    
    client.print("Temperature: ");
    client.print(sensor.getTempCByIndex(0));
    client.print(" C ");
    client.print(sensor.getTempFByIndex(0));
    client.print(" F");
    client.println("</html>");
}

void showHome(WiFiClient client){
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("Welcome to Home AS");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}

void checkIntruderGarden(){
  int lightSensor = analogRead(analogPin);
  if(lightSensor > 300){
    analogWrite(RLED, random(0,255));
    analogWrite(GLED, random(0,255));
    analogWrite(BLED, random(0,255));
    intruder = true;
  }
  else{
    analogWrite(RLED, 0);
    analogWrite(GLED, 0);
    analogWrite(BLED, 0);
    intruder = false;
  }
  
}

void checkIntruderGarden(WiFiClient client){
  int lightSensor = analogRead(analogPin);
  Serial.print("Reading sensor: ");
  Serial.println(lightSensor);
  if(lightSensor > 300){
    Serial.println("INTRUDER ALERT! GET OUT OF MY LAWN!");
    analogWrite(RLED, random(0,255));
    analogWrite(GLED, random(0,255));
    analogWrite(BLED, random(0,255));
    intruder = true;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.print("Intruder Alert: ");
    client.println(" WARNING WARNING WARNING!");
   
    client.println("</html>");
  }
  else{
    analogWrite(RLED, 0);
    analogWrite(GLED, 0);
    analogWrite(BLED, 0);
    intruder = false;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.print("Intruder Alert: ");
    client.println("SAFE!");
   
    client.println("</html>");
  }
}

String getTime(WiFiClient client) {
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");
 
  while(!!!client.available()) {
     yield();
  }

  while(client.available()){
    if (client.read() == '\n') {    
      if (client.read() == 'D') {    
        if (client.read() == 'a') {    
          if (client.read() == 't') {    
            if (client.read() == 'e') {    
              if (client.read() == ':') {    
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                return theDate;
              }
            }
          }
        }
      }
    }
  }
}


void getAllData(WiFiClient client){
  sensor.requestTemperatures(); //Request temperatures from the sensor
  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject(); 
  
  if(intruder) JSONencoder["intruderAlert"] = "WARNING!";
  else JSONencoder["intruderAlert"] = "SAFE!";
  JSONencoder["temperatureCelcius"] = sensor.getTempCByIndex(0);
  JSONencoder["temperatureFahrenheit"] = sensor.getTempFByIndex(0);
  JSONencoder["Time"] = getTime(client) ;
  
  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
  
  HTTPClient http;    //Declare object of class HTTPClient
  
  http.begin("http://flip3.engr.oregonstate.edu:3000/postjson");      
  http.addHeader("Content-Type", "application/json"); 
  
  int httpCode = http.POST(JSONmessageBuffer); 
  String payload = http.getString();                                       
  
  Serial.println(httpCode);  
  Serial.println(payload);  

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(JSONmessageBuffer);  
  client.println("</html>");
  
  http.end();
  Serial.println(JSONmessageBuffer);
  
}

void startLaser(){
  digitalWrite(LASER, HIGH); //Set pin 7 to HIGH
 // delay(delayTime); //wait defined time period before repeating
 // digitalWrite(LASER, LOW); //Set pin 7 to LOW
 // delay(delayTime); //wait defined time period before repeating

}

