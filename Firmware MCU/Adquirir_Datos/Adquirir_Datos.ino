/*
  Date: 2023/06/28
  Autor: Carlos Rodríguez
  
  This sketch version uses MQTT protocol communication to send sensor data
  to a Node-RED dashboard via SIM7600 module.
  Include readings from UV, soil moisture and humidity/temperature sensors.
  Updates:
  - Use 3.25 reference voltage for UV sensor readings
  - Instead of the SHTC3 sensor, use the HDC1080 humidity and temperature sensor
  - Rain sensor implemented
*/

const char* ver = "Version 0.5";
const char* sketch = "MQTTSIM7600_V_0.5";

// Libraries ///////////////////////
//#include "SparkFun_SHTC3.h"
#include "ClosedCube_HDC1080.h"
#include "Wire.h"
#include <Arduino.h>
//#include <ML8511.h>

//SIM7600 interface/////////////////
#define SerialSIM Serial2 // SIM7600 Serial interface
#define RX2 16            // ESP32 UART2 Rx pin 
#define TX2 17            // ESP32 UART2 Tx pin
#define PWR 4             // SIM7600 power key pin
#define SLP 2             // SIM7600 sleep pin

//UV ML8511 sensor//////////////////
#define UVOUT   33     // Output from the sensor.
#define REF_3V3 36     // 3.3V power on the Arduino board.
#define EN      32     // Enable sensor.

//Soil moisture sensor//////////////
const int AirValue = 2881;   //2904
const int WaterValue = 1136; //1122

//int soilMP=0;
byte measure=100;
byte state=0;
#define Asensor 34

//Rain sensor///////////////////////

#define inRain  23
#define anRain  12

// Instances ///////////////////////
//SHTC3 mySHTC3;                      // Humidity and temperature sensor (SHTC3) instance
ClosedCube_HDC1080 hdc1080;
//ML8511 light(ANALOGPIN, ENABLEPIN); // UV ML8511 sensor instance 

// MQTT ////////////////////////////
String clientId = "esp1";
const char* broker = "146.190.39.154";          // Public IP
const char* mqttUsername = "carodriguezc87";    // MQTT username
const char* mqttPassword = "mqttLaberinto1026"; // MQTT password

const char* topicAirTemp = "esp/airTemp";       // Air temperature topic (°C)
const char* topicHumidity = "esp/humidity";     // Humidity topic        (%)
const char* topicMoisture = "esp/moisture";     // Soil moisture topic   (%)
const char* topicUV = "esp/UV";                 // UV topic              (mW/cm2)
const char* sensorsTopic = "esp/sensors";       // UV topic     
////////////////////////////////////

// Variables ///////////////////////
String airTemp = "";      // Store temperature values
String humidity = "";     // Store humidity values
String UV = "";           // Store UV readings
String soilMP = "";        // Store soil moisture values
String rain = "";
String rainAn = "";
String battery = "0";
String rssi = "0";
String pLength = "";
String dSens = "";
unsigned long prev_time=0;
unsigned long curr_time=0;
unsigned long minuts=60000;


uint32_t start;
uint32_t stoped;

// Functions ///////////////////////
void MQTT_Init();
void MQTT_Subscribe();
void MQTT_Publish(String Tlength, String topic,String Plength, float payLoad);
void printInfo();


void setup(){
  delay(1000);
  Serial.begin(9600);
  while(Serial == false);                       // Wait for the serial connection to start up
  Serial.println(F(ver));                       // Sketch vertion
  Serial.println(F(sketch));                    // Sketch name loaded
  delay(1000);
  Serial.println("System ready");
  Wire.begin(); 
  SerialSIM.begin(115200,SERIAL_8N1,RX2,TX2);   // Begin serila connection with SIM7600
  while(Serial == false);  
                                    // Initialize MQTT settings configuration
  //MQTT_Subscribe();
  
  //Wire.begin();
  //Serial.print("Beginning SHTC3 sensor. Result = ");  // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution 
  // errorDecoder(mySHTC3.begin());                      // Start SHTC3 sensor
  hdc1080.begin(0x40);
  pinMode(inRain,INPUT);
  pinMode(EN,OUTPUT);
  pinMode(REF_3V3, INPUT);
  Serial.println();
  delay(5000);                                  
}

void loop(){
  //curr_time = millis();
  MQTT_Init();
  //SHTC3Read();
  HDCRead();
  UVRead();
  moistureRead();
  rainRead();
  dSens ="{\"temp\":" + airTemp + ",\"humid\":" + 
          humidity + ",\"uv\":" + UV + ",\"s_moist\":" + 
          soilMP + ",\"rain\":" + rain + ",\"rainAn\":" + rainAn +
          ",\"bat\":" + battery + ",\"rssi\":" + rssi + "\}";
          
  MQTT_Publish(sensorsTopic,dSens);
  mqttDisconnect();
  //curr_time = millis() - curr_time; 
  for(int i=0;i<10;i++)
    delay(minuts);
}

