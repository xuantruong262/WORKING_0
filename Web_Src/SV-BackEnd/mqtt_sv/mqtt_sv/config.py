from pymongo import MongoClient

MQTT_SERVER = 'ngoinhaiot.com'
MQTT_PORT = 2222
MQTT_KEEPALIVE = 60
MQTT_USER = 'XuanTruong'
MQTT_PASSWORD = '8FEB9F35E8D44666'

# MQTT_SERVER = 'maqiatto.com'
# MQTT_PORT = 8883
# MQTT_KEEPALIVE = 60
# MQTT_USER = 'vxtruong262@gmail.com'
# MQTT_PASSWORD = '1'

MQTT_CONFIG = {
    'server': MQTT_SERVER,
    'port': MQTT_PORT,
    'keep_alive': MQTT_KEEPALIVE,
    'username': MQTT_USER,
    'password': MQTT_PASSWORD
}

MQTT_TOPICS = {
    # 'ph': MQTT_USER + '/PH_value',
    # 'tds': MQTT_USER + '/TDS_value',
    # 'temp': MQTT_USER + '/Temprature_value',
    'id': MQTT_USER + '/123456789',
    'sp': MQTT_USER + '/sp',
    'setpoint':  MQTT_USER + '/setpoint',
    'connect':  MQTT_USER + '/connection'
}

MONGO_CLIENT = MongoClient()
DB = MONGO_CLIENT['mqtt']
MQTT_COLLECTION = DB['mqtt']