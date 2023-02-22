MQTT_SERVER = 'maqiatto.com'
MQTT_PORT = 1883
MQTT_KEEPALIVE = 60
MQTT_USER = 'vxtruong262@gmail.com'
MQTT_PASSWORD = '1'

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
    'temp': MQTT_USER + '/Temprature_value'
}