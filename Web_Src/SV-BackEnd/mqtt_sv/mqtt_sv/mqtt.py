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
        mqtt_client.subscribe(topics['device'])
    else:
        print('Bad connection. Code:', rc)


def on_message(mqtt_client, userdata, msg):
    print(f'Received message on topic: {msg.topic} with payload: {msg.payload}')
    global receive_time
    try:
        if receive_time == 180:
            device = json.loads(msg.payload.decode("utf-8"))
            res = {
                "saved_time": toDateTimeObj(datetime.datetime.today()),
                "topic": msg.topic,
                "device_id": int(device['ID']),
                "ph": float(device['PH']),
                "tds": int(device['TDS']),
                "temperature": float(device['Temp'])
            }
            mqtt_collection.insert_one(res)
            print('Inserted successfully', res)
            receive_time = 0
        receive_time += 1
    except Exception as e:
        print('Error: Exception with:', e)
        receive_time = 0

def toDateTimeObj(date):
    return {
        'day': date.day,
        'month': date.month,
        'year': date.year
    }

client = mqtt.Client("phuoc", transport='websockets')
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(mqtt_config['username'], mqtt_config['password'])
client.connect(
    host=mqtt_config['server'],
    port=mqtt_config['port'],
    keepalive=mqtt_config['keep_alive']
)
