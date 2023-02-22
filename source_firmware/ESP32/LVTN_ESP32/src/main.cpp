#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "MQTT_Connection.h"
#include <string.h>

#define TX2 
#define RX2
String str;

float PH = 0 ,temperature = 0;
int   TDS = 0;

float PH_Setpoint = 0, \
      PH_THR_Setpoint, \
      TDS_Setpoint =0, \
      TDS_THR_Setpoint = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(115200);
  Serial.setTimeout(500);
  setup_wifi();
  MQTT_Init();
  connect_to_broker();
}

String splitString(String str, String delim, uint16_t pos)
{
  String tmp = str;
  for(int i=0; i<pos; i++)
  {
    tmp = tmp.substring(tmp.indexOf(delim)+1);    
    if(tmp.indexOf(delim)== -1 && i != pos -1 )
      return "";
  }
  return tmp.substring(0,tmp.indexOf(delim));
}

void filt_data()
{
  String str1,str2,str3;
  str1 = splitString(str,",",1);
  str2 = splitString(str,",",2);
  str3 = splitString(str,",",3);
  PH = str1.toFloat();
  TDS= str2.toInt();
  temperature = str3.toFloat();
}



void loop() {
 // put your main code here, to run repeatedly:
  MQTT_loop();
  // if(Serial2.available())
  // {
  //   str = Serial2.readString();   
  //   filt_data();
  //   publish_data();
  // }
  // PH++;
  // TDS++;
  // temperature++;
  // publish_data();


} 