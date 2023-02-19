#include"MQTT_Connection.h"
#include<PubSubClient.h>
#include<WiFi.h>

extern float PH;
extern float temperature;
extern int TDS;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* ssid = "Xuan Truong";
const char* password = "3conchimnon";
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connect_to_broker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(Topic_1);
      client.subscribe(Topic_2);
      client.subscribe(Topic_3);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  char status[20];
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
  for(int i = 0; i<length; i++)
  {
    status[i] = payload[i];
  }
  Serial.println(status);
  //client.publish(Topic_1,status);
}
void MQTT_Init()
{
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT );
  client.setCallback(callback);
  connect_to_broker();
  Serial.println("Start transfer");
}

void publish_data()
{
  char Temp_Buffer[50];
  dtostrf(PH, 1, 0, Temp_Buffer);
  client.publish(Topic_1,Temp_Buffer);
  memset(Temp_Buffer,0,strlen(Temp_Buffer));

  dtostrf(TDS, 1, 0, Temp_Buffer);
  client.publish(Topic_2,Temp_Buffer);
  memset(Temp_Buffer,0,strlen(Temp_Buffer));

  dtostrf(temperature, 1, 0, Temp_Buffer);
  client.publish(Topic_3,Temp_Buffer);
  memset(Temp_Buffer,0,strlen(Temp_Buffer));


}
void MQTT_loop()
{
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }

}