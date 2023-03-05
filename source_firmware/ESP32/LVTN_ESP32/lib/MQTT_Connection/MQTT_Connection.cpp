#include"MQTT_Connection.h"


WiFiClient wifiClient;
PubSubClient client(wifiClient);

extern String kaka;

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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  char status[100];
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
  char temp_string[100];
  kaka.toCharArray(temp_string,kaka.length()+1);
  client.publish(Topic_1,temp_string,kaka.length()+1);
}
void MQTT_loop()
{
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }

}