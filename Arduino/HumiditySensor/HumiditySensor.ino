#include <DHT.h>
DHT dht(6, DHT11); 

void setup() {
 Serial.begin(9600);
 dht.begin(); 
}
void loop() {
  digitalWrite(5, HIGH);
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

