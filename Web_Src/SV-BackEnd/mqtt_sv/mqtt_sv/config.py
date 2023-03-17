from pymongo import MongoClient

MQTT_SERVER = 'ngoinhaiot.com'
MQTT_PORT = 2222
MQTT_KEEPALIVE = 60
MQTT_USER = 'XuanTruong'
MQTT_PASSWORD = '8FEB9F35E8D44666'

MQTT_CONFIG = {
    'server': MQTT_SERVER,
    'port': MQTT_PORT,
    'keep_alive': MQTT_KEEPALIVE,
    'username': MQTT_USER,
    'password': MQTT_PASSWORD
}

MQTT_TOPICS = {
    'ph': MQTT_USER + '/PH_value',
    'tds': MQTT_USER + '/TDS_value',
    'temp': MQTT_USER + '/Temprature_value',
    'device': MQTT_USER + '/123456789'
}

HOST = 'mongodb+srv://phuoc:0909605306@mqtt.tofdq9n.mongodb.net/test'

MONGO_CLIENT = MongoClient(HOST)
DB = MONGO_CLIENT['mqtt']
MQTT_COLLECTION = DB['mqtt']