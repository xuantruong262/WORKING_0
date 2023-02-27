#ifndef _MQTT_CONNECTION_H_
#define _MQTT_CONNECTION_H_

#include<PubSubClient.h>
#include<WiFi.h>
// #define MQTT_SERVER "maqiatto.com"
// #define MQTT_PORT 1883
// #define MQTT_USER "vxtruong262@gmail.com"
// #define MQTT_PASSWORD "1"
// #define Topic_1 "vxtruong262@gmail.com/123456789"



#define MQTT_SERVER "ngoinhaiot.com"
#define MQTT_USER "XuanTruong"
#define MQTT_PASSWORD "759E283B03B34284"
#define MQTT_PORT 1111
#define Topic_1 "XuanTruong/123456789" 


void setup_wifi();
void connect_to_broker();
void MQTT_Init();
void publish_data();
void MQTT_loop();


#endif