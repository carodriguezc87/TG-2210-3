//Lectura del sensor de humedad y temperatura.
bool HDCRead(){
  float hum = hdc1080.readHumidity();
  hum = floatMap(hum,44.78,78.85,33,75);
  delay(20);
  airTemp = String(hdc1080.readTemperature());
  if(hum>100)hum=100;
  humidity = String(hum);
  Serial.print(humidity);
  Serial.print(" %");
  Serial.print("\t");
  Serial.print(airTemp);
  Serial.println("°C");
}
float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Lectura del sensor de radiación UV
bool UVRead(){
  digitalWrite(EN,HIGH);
  //int uvLevel = averageAnalogRead(UVOUT);
  delay(2);
  int uvLevel = analogRead(UVOUT);
  digitalWrite(EN,HIGH);
  //int refLevel = averageAnalogRead(REF_3V3);
  //float outputVoltage = (3.3 / refLevel) * uvLevel;
  Serial.print("Voltaje");
  Serial.println(uvLevel);
  float outputVoltage = (3.3 * uvLevel/4095)+0.13;
  if(outputVoltage>=1.1)outputVoltage+=0.28;
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.4, 0.0, 15.0); //Convert the voltage to a UV intensity level
  if(uvIntensity<0)uvIntensity=0;
  UV = String(uvIntensity);
  Serial.print(UV);
  Serial.println(" mW cm^2");
  return true;
}

int averageAnalogRead(int pinToRead){
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 
  for(int x = 0 ; x < numberOfReadings ; x++){
    runningValue += analogRead(pinToRead);
    delay(5);
  } 
  runningValue /= numberOfReadings;
  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Lectura del sensor de humedad del suelo
bool moistureRead(){
  float soilM = 0;
  float soilMPA=0;
  for(byte i=0;i<measure;i++){
    soilM += analogRead(Asensor);  //put Sensor insert into soil
    delay(20);
  }
  soilM /= measure; 
  //Serial.println(soilM);
  soilMPA = map(soilM, AirValue, WaterValue, 0, 100);
  if(soilMPA >= 100){
    Serial.println("100 %");
    soilMPA=100;
  }
  else if(soilMPA <=0){
    soilMPA=0;
    Serial.println("0 %");
  }
  else if(soilMPA >=0 && soilMPA <= 100){
    if (soilM<=1353) Serial.print("wet:      ");
    else if (soilM<=1790) Serial.print("moist:    ");
    else if (soilM<=2226) Serial.print("dry:      ");
    else if (soilM<=2662) Serial.print("very dry: ");
    else {
      Serial.print("air:      ");
    }
    Serial.print(soilMPA);
    Serial.println("%"); 
   }
  soilMP=String(soilMPA);
  return true;
}

//Lectura del sensor de lluvia
bool rainRead (){
  int rainVal=0;
  for(int i=0; i<5; i++){
    if(digitalRead(inRain)) rain = "0";
    else {
      rain = "10";
      i=5;
    }
    delay(10);
  }
  rainVal = analogRead(anRain);
  rainVal = map(rainVal, 0, 4095, 100, 0);
  rainAn = String(rainVal);
  return true;
}
