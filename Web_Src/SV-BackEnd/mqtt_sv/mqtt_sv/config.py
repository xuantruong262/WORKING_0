MQTT_SERVER = 'ngoinhaiot.com'
MQTT_PORT = 2222
MQTT_KEEPALIVE = 60
MQTT_USER = 'XuanTruong'
MQTT_PASSWORD = '759E283B03B34284'

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