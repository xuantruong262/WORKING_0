#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "MQTT_Connection.h"
#include <string.h>
#include <esp_task_wdt.h>

#define TX2 
#define RX2
#define WDT_TIMEOUT 10
#define test_led 2

String kaka;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(115200);
  Serial.setTimeout(500);
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  pinMode(test_led,OUTPUT);

  for(int i=0;i<5;i++)
  {
    digitalWrite(test_led,1);
    delay(100);
    digitalWrite(test_led,0);
  }

  setup_wifi();
  MQTT_Init();
  connect_to_broker();

  
}

// String splitString(String str, String delim, uint16_t pos)
// {
//   String tmp = str;
//   for(int i=0; i<pos; i++)
//   {
//     tmp = tmp.substring(tmp.indexOf(delim)+1);    
//     if(tmp.indexOf(delim)== -1 && i != pos -1 )
//       return "";
//   }
//   return tmp.substring(0,tmp.indexOf(delim));
// }




void loop() {
 // put your main code here, to run repeatedly:
  MQTT_loop();

  // if ((unsigned long)(millis() - time) >= 3000) 
  // {
  //   esp_task_wdt_reset();
  //   time = millis();
  // }
  if(Serial2.available())
  {
    kaka = Serial2.readString(); 
    publish_data();
  }
  esp_task_wdt_reset();



} 