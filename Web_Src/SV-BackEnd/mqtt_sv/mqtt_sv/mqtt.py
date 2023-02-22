import paho.mqtt.client as mqtt
from django.conf import settings
from . import config

mqtt_config = config.MQTT_CONFIG
topics = config.MQTT_TOPICS


def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print('Connected with broker successfully')
        mqtt_client.subscribe([(topics['ph'], 0), (topics['tds'], 0), (topics['temp'], 0)])
    else:
        print('Bad connection. Code:', rc)


def on_message(mqtt_client, userdata, msg):
    print(f'Received message on topic: {msg.topic} with payload: {msg.payload}')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(mqtt_config['username'], mqtt_config['password'])
client.connect(
    host=mqtt_config['server'],
    port=mqtt_config['port'],
    keepalive=mqtt_config['keep_alive']
)

