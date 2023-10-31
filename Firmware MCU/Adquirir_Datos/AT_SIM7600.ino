void MQTT_Init(){ //
  Serial.println(F("Connecting to server..."));  
  SerialSIM.println("ATE0");
  delay(2000);
  SerialSIM.println("AT+CMQTTSTART");           //Establishing MQTT Connection
  delay(2000); 
  SerialSIM.println("AT+CMQTTACCQ=0,\"esp1\",0,4"); // Acquire a clientClient: index(0-1), client ID
  //SerialSIM.println("AT+CMQTTACCQ=0,\"" + clientId);
  delay(2000);
  SerialSIM.println("AT+CMQTTCONNECT=0,\"tcp://146.190.39.154:1883\",90,0,\"carodriguezc87\",\"mqttLaberinto1026\""); //MQTT Server Name for connecting this client
  
  //SerialSIM.println("AT+CMQTTCONNECT=0,\"tcp://146.190.39.154\",90,0,\"carodriguezc87\",\"mqttLaberinto1026\""); //MQTT Server Name for connecting this client

  delay(2000);
  Serial.println("SIM7600 initialized");
}

void MQTT_Subscribe(){
  SerialSIM.println("AT+CMQTTSUBTOPIC=0,11,1"); //AT Command for Setting up the Subscribe Topic Name 
  delay(2000);
  SerialSIM.println(topicAirTemp);              //Topic Name
  delay(2000);
  SerialSIM.println("AT+CMQTTSUB=0,8,1,1");     //Length of message
  delay(2000);
  SerialSIM.println("Air_temp");                //message
  delay(2000);
  SerialSIM.println("AT+CMQTTSUBTOPIC=0,12,1"); //AT Command for Setting up the Subscribe Topic Name 
  delay(2000);
  SerialSIM.println(topicHumidity);             //Topic Name
  delay(2000);
  SerialSIM.println("AT+CMQTTSUB=0,8,1,1");     //Length of message
  delay(2000);
  SerialSIM.println("Humidity");                //message
  delay(2000);
  Serial.println("Done");
}

void MQTT_Publish(String topic,String payLoad){
  Serial.println("Publishing to " + topic + ": " + payLoad);
  SerialSIM.println("AT+CMQTTTOPIC=0," + String(topic.length()) +""); //AT Command for Setting up the Publish Topic Name 
  //SerialSIM.println("AT+CMQTTTOPIC=0,11");
  delay(1000);
  SerialSIM.println(topic); //Topic Name
  delay(1000);
  SerialSIM.println("AT+CMQTTPAYLOAD=0," + String(payLoad.length())+"");
  //SerialSIM.println("AT+CMQTTPAYLOAD=0,2");
  delay(1000);
  SerialSIM.println(payLoad); //Payload message
  delay(1000);
  SerialSIM.println("AT+CMQTTPUB=0,1,60"); //Acknowledgment
  delay(1000);
}
void MQTT_PublishB(String topic,String payLoad){
  Serial.println("Publishing to " + topic + ": " + payLoad);
  SerialSIM.println("AT+CMQTTTOPIC=0," + String(topic.length()) +""); //AT Command for Setting up the Publish Topic Name 
  delay(1000);
  SerialSIM.println(topic); //Topic Name
  delay(1000);
  SerialSIM.println("AT+CMQTTPAYLOAD=0," + String(payLoad.length())+"");
  delay(1000);
  SerialSIM.println(payLoad);  // Payload message
  delay(1000);
  SerialSIM.println("AT+CMQTTPUB=0,1,60"); //Acknowledgment
  delay(1000);
}

void mqttDisconnect() {
  Serial.println("Diconnect MQTT...");
  SerialSIM.println("AT+CMQTTDISC=0,60"); //Disconnect Server
  delay(2000);
  Serial.println("done!");

  Serial.println("Release client...");
  SerialSIM.println("AT+CMQTTREL=0"); //Release Client
  delay(2000);
  Serial.println("done!");

  Serial.println("Stop MQTT...");
  SerialSIM.println("AT+CMQTTSTOP"); //Stop 4G connection
  delay(2000);
  Serial.println("done!");
}
