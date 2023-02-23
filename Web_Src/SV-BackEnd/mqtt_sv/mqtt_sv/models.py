from mongoengine import *


class MqttTopicValue(Document):
    topic = StringField()
    value = StringField()