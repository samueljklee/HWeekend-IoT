
#define LMINUS D6
#define LPLUS D7
#define AnalogPin A0

void setup() {
  pinMode(LMINUS,INPUT);
  pinMode(LPLUS,INPUT);
  pinMode(AnalogPin, OUTPUT);

}

void loop() {
  Serial.begin(9600);
  //Debugging
  Serial.println(digitalRead(LMINUS));
  Serial.println(digitalRead(LPLUS));
  Serial.println(analogRead(AnalogPin));
  
  if((digitalRead(LMINUS)==1)||(digitalRead(LPLUS)==1)){
      Serial.println("!");
  }
  else{
      Serial.println(analogRead(AnalogPin));
  }
  delay(100);
  Serial.end();
}


