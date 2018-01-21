#include <OneWire.h>
#include <DallasTemperature.h>
 
#define ONEWIRE D3

OneWire oneWire(ONEWIRE);
DallasTemperature sensors(&oneWire);
 
void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
}
 
 
void loop(void)
{
  Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0)); 
 
}
