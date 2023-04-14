import paho.mqtt.client as mqtt
from websocket import create_connection
import websocket
import requests
from django.conf import settings
from . import config, models
from mqtt_fe import views
from mqtt_fe import consumers
import re
from time import sleep
import threading

import datetime, json
mqtt_config = config.MQTT_CONFIG
topics = config.MQTT_TOPICS
mqtt_collection = config.MQTT_COLLECTION

my_string = ''

receive_time = 1

def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print('[' + datetime.datetime.now().strftime('%c') + '] ' + 'Connected with broker successfully')
        # mqtt_client.subscribe([(topics['ph'], 0), (topics['tds'], 0), (topics['temp'], 0)])
        mqtt_client.subscribe([(topics['id'], 0), (topics['setpoint'], 0), (topics['connect'], 0)])
    else:
        print('Bad connection. Code:', rc)
def on_message(mqtt_client, userdata, msg):
    # print(f'Received message on topic: {msg.topic} with payload: {msg.payload}')
    global my_string
    my_string = msg.payload.decode("UTF-8")
    if(my_string == "connect"):
        print("check connecting")
    else:    
        try:
            global my_data
            my_data = my_string
            my_data = json.loads(msg.payload.decode("UTF-8"))
            ws = create_connection('ws://127.0.0.1:8000/ws/xuantruong/') 
            global data
            try:
                data = {
                "PH": my_data['PH'],
                "TDS": my_data['TDS'],
                "TEMP": my_data['Temp'],
                }
            except:
                print("Missing Temp")   


            ws.send('update')
            ws.close()
            
        except ValueError as e:
            print("Wrong JSON String")





# def on_publish(client,userdata,mid):


client = mqtt.Client("phuoc", transport='websockets')
client.on_connect = on_connect
client.on_message = on_message
# client.on_publish = on_publish
client.username_pw_set(mqtt_config['username'], mqtt_config['password'])
client.connect(
    host=mqtt_config['server'],
    port=mqtt_config['port'],
    keepalive=mqtt_config['keep_alive']
)
def check_connection():
    while True:
        global my_string
        global flag
        if(my_string == "connect"):
            my_string ="nothing"
            print("still connecting")
            flag = True
        else:
            print("disconnecting!!!")
            flag =False

        ws = create_connection('ws://127.0.0.1:8000/ws/xuantruong/') 
        ws.send('update')
        ws.close()
           
        sleep(10)


stop_threads = False
timer_thread = threading.Thread(target= check_connection)
timer_thread.start()

def stop_func():
    timer_thread.join()

