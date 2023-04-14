#include <Arduino.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFi.h>
#include "MQTT_Connection.h"
#include <string.h>
#include <esp_task_wdt.h>
#include <WiFiManager.h>


#define TX2 
#define RX2
#define WDT_TIMEOUT 20
#define test_led 2

String data_receive;
String username;
String pass;
// char aSSID[100] = {0};
// char aPASSWORD[100]={0};
void Write_to_EPP(String wifi_name,String wifi_password)
{
  // uint32_t wifi_name_length = strlen(wifi_name);
  // uint32_t wifi_password_length = strlen(wifi_password);
  EEPROM.writeString(0,wifi_name);                      //address = 0x00
  EEPROM.writeString(0x64,wifi_password);               //address = 0x64
  EEPROM.commit();
}

String Read_from_EEP(unsigned int address)
{
  String save_string;
  save_string =  EEPROM.readString(address); 
  return save_string;
}
char my_str[100];
char *handle_string(String str)
{
  memset(my_str,0,strlen(my_str));
  str.toCharArray(my_str,str.length());
  return my_str;
}

void Config_Wifi()
{
  bool res;
  WiFi.mode(WIFI_STA);
  WiFiManager cWifi;
  cWifi.resetSettings();
  res = cWifi.autoConnect("MyHydroponic","99999999"); // password protected ap
  if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
  else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
        Serial2.write("Config_Wifi_done");
        username = cWifi.getWiFiSSID();
        pass = cWifi.getWiFiPass();
        Write_to_EPP(username,pass);
    }
}
void setup() {
  Serial.begin(9600);
  //============================================TESTING
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("MyHydroponic","99999999");
  //============================================ EEPROM
  Serial2.begin(115200); 
  // Serial.setTimeout(500);
  // EEPROM.begin(512);
  // Write_to_EPP("XuanTruong","3conchimnon");
  // EEPROM.commit();
//=========================================== WatchDog
  // esp_task_wdt_init(WDT_TIMEOUT, true);  
  // esp_task_wdt_add(NULL);

  pinMode(test_led,OUTPUT); 

  for(int i=0;i<5;i++)
  {
    digitalWrite(test_led,1);
    delay(100);
    digitalWrite(test_led,0);
  }
//=========================================== WIFI AND MQTT                                     
  // setup_wifi();
  MQTT_Init();
  connect_to_broker();

  
}

void loop() {

  MQTT_loop();
  if(Serial2.available())
  {
    data_receive =  Serial2.readString();
    publish_data(data_receive);
    if(data_receive == "wificonfigmode")
    {
       Serial.println(Serial2.readString());
       Config_Wifi();
    }
  }
  // esp_task_wdt_reset();
  
  // Serial.println(username + pass);
  // delay(2000);

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