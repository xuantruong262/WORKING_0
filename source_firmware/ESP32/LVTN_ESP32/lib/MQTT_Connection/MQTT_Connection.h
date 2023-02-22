#ifndef _MQTT_CONNECTION_H_
#define _MQTT_CONNECTION_H_

#define MQTT_SERVER "maqiatto.com"
#define MQTT_PORT 1883
#define MQTT_USER "vxtruong262@gmail.com"
#define MQTT_PASSWORD "1"
#define Topic_1 "vxtruong262@gmail.com/PH_value"
#define Topic_2 "vxtruong262@gmail.com/TDS_value"
#define Topic_3 "vxtruong262@gmail.com/Temprature_value"

// #define MQTT_SERVER "ngoinhaiot.com"
// #define MQTT_USER "XuanTruong"
// #define MQTT_PASSWORD "759E283B03B34284"
// #define MQTT_PORT 1111
// #define Topic_1 "XuanTruong/PH_value"
// #define Topic_2 "XuanTruong/TDS_value"
// #define Topic_3 "XuanTruong/Temprature_value"   


void setup_wifi();
void connect_to_broker();
void MQTT_Init();
void publish_data();
void MQTT_loop();


#endif