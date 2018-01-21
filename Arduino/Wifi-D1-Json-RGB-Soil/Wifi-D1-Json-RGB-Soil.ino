#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "FastLED.h"

#define ledPin D4
//Soil
#define analogPin A0
int thresholdValue = 700;

//RGB
#define DATA_PIN    D5
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    7
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

//Wifi Setting
const char* ssid = "robot";
const char* password = "";
WiFiServer server(80); 


void setup() 
{
  Serial.begin(9600);
    
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
  delay(10);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");


  //RGB
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); 
  FastLED.setBrightness(BRIGHTNESS);
}

//RGB
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
 
void loop() 
{   
  WiFiClient client = server.available();
  Serial.println("new client");
  
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/LED=ON") != -1) {
    Serial.println("LED ON");
    digitalWrite(ledPin, HIGH);
    ledFunc(client);
  } 
  if (request.indexOf("/LED=OFF") != -1){
    Serial.println("LED ON");
    digitalWrite(ledPin, LOW);
    ledFunc(client);
  }
  if(request.indexOf("/getAllData") != -1){
      getAllDataFunc(client);
  }
  if(request.indexOf("/rgb") != -1){
    rgbFunc();
  }
  if(request.indexOf("/soil") != -1){
    soilFunc(client);
  }
  
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
  
}

void soilFunc(WiFiClient client){
  int sensorValue = analogRead(analogPin);
  Serial.print(sensorValue);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  if(sensorValue < thresholdValue){
    Serial.println("Enough Water!");
     client.println("Enough Water!");
  }
  else {
    Serial.println("Time to water the plants!");
    client.println("Time to water the plants!");
  }
 
  client.println("</html>");
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

void getAllDataFunc(WiFiClient client){
  int soilSensor = analogRead(analogPin);
  
  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject(); 

  JSONencoder["Soil"] = soilSensor;
  JSONencoder["Time"] = "10:10";
  
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
  client.println("<br><br>");
  client.println("Get all Data");
  client.println(JSONmessageBuffer);  
  client.println("</html>");
  
  http.end();
}



//RGB Functions
void rgbFunc(){
  //RGB
  gPatterns[gCurrentPatternNumber]();
  FastLED.show(); 
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

