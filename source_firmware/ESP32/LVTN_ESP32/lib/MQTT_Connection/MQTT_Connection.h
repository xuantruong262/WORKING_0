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
#define MQTT_PASSWORD "8FEB9F35E8D44666"
#define MQTT_PORT 1111
#define Topic_1 "XuanTruong/26022000" 


void setup_wifi();
void connect_to_broker();
void MQTT_Init();
void publish_data(String my_string);
void MQTT_loop();


#endif