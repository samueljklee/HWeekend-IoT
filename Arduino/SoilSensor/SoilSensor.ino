#include <DHT.h>

#define analogPin A0
#define led1 6
#define led2 7 

DHT dht(5, DHT11); 

int analogPin = A0;
int led1 = 6;
int led2 = 7;
int thresholdValue = 700;

void setup(){
  Serial.begin(9600);
  dht.begin(); 
  pinMode(analogPin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  
}

void loop() {
  int sensorValue = analogRead(analogPin);
  Serial.print(sensorValue);
  if(sensorValue < thresholdValue){
    Serial.println("Over 700");
    digitalWrite(led2, LOW);
    digitalWrite(led1, HIGH);
  }
  else {
    Serial.println("Under 700");
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
  }
  
  float humidity = dht.readHumidity(); 
  float temperature = dht.readTemperature(); // Celcius
  if (isnan(humidity) || isnan(temperature)) { 
    Serial.println("Error reading");
    return; 
  }
  float heatIndex = dht.computeHeatIndex(temperature,humidity,false);
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" Temperature: ");
  Serial.print(temperature);
  Serial.print(" Heat Index: ");
  Serial.println(heatIndex);
  delay(500); 
}
