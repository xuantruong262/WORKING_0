import paho.mqtt.client as mqtt
from django.conf import settings
from . import config, models

import datetime, json

mqtt_config = config.MQTT_CONFIG
topics = config.MQTT_TOPICS
mqtt_collection = config.MQTT_COLLECTION

receive_time = 1


def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print('[' + datetime.datetime.now().strftime('%c') + '] ' + 'Connected with broker successfully')
        mqtt_client.subscribe([(topics['ph'], 0), (topics['tds'], 0), (topics['temp'], 0)])
    else:
        print('Bad connection. Code:', rc)


def on_message(mqtt_client, userdata, msg):
    print(f'Received message on topic: {msg.topic} with payload: {msg.payload}')
    global receive_time
    try:
        if receive_time == 1:
            device = json.loads(msg.payload.decode("utf-8"))
            res = {
                "timestamp": datetime.datetime.now(),
                "topic": msg.topic,
                "device_id": device['id'],
                "ph": device['ph'],
                "tds": device['tds'],
                "temperature": device['temp']
            }
            mqtt_collection.insert_one(res)
            print('Inserted successfully', res)
            receive_time = 0
        receive_time += 1
    except Exception as e:
        print('Error: Exception with:', e)


client = mqtt.Client("phuoc", transport='websockets')
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(mqtt_config['username'], mqtt_config['password'])
client.connect(
    host=mqtt_config['server'],
    port=mqtt_config['port'],
    keepalive=mqtt_config['keep_alive']
)
