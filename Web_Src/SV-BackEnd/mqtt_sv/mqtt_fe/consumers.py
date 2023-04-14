from channels.consumer import SyncConsumer,AsyncConsumer
from channels.generic.websocket import AsyncWebsocketConsumer,WebsocketConsumer
from asgiref.sync import async_to_sync
from channels.exceptions import StopConsumer
from time import sleep
import asyncio
import json
from mqtt_sv import mqtt


# WebsocketConsumer



class Consumer(WebsocketConsumer):
    def connect(self):
        self.group_name = 'VXT'
        async_to_sync(self.channel_layer.group_add)(
            self.group_name,
            self.channel_name
        )
        self.accept()
                    # GOOD FUNCTION
    def receive(self, text_data):
        print(text_data)
        if(text_data == "update"):
            async_to_sync(self.channel_layer.group_send)(
                self.group_name,
                {
                    'type': 'update',
                }
            ) 
        # elif(text_data == "state"):
        #     async_to_sync(self.channel_layer.group_send)(
        #         self.group_name,
        #         {
        #             'type': 'state',
        #         }
        #     ) 
    def update(self,event):
        self.send(json.dumps({
            "PH":mqtt.data['PH'],
            "TDS":mqtt.data['TDS'],
            "TEMP":mqtt.data['TEMP'],
            "STATE":mqtt.flag,
        }))

    # def state(self,event):
    #     print(mqtt.flag)
    #     self.send(json.dumps({
    #         "PH":mqtt.data['PH'],
    #         "TDS":mqtt.data['TDS'],
    #         "TEMP":mqtt.data['TEMP'],
    #         "STATE":mqtt.flag,
    #     }))
    

                     # TESTING
    # def receive(self, text_data):
    #     print('>>>>>>>>>>',text_data)
    #     data = json.loads(text_data)
    #     PH = data['PH']
    #     TDS =data['TDS']
    #     TEMP = data['TEMP']
    #     async_to_sync(self.channel_layer.group_send)(
    #         self.group_name,
    #             {
    #                 'type': 'update',
    #                 'my_ph':PH,
    #                 'my_tds':TDS,
    #                 'my_temp':TEMP,
    #             }
    #         )
    
    # def update(self,event):
    #     ph = event['my_ph']
    #     tds =event['my_tds']
    #     temp = event['my_temp']
    #     self.send(json.dumps({
    #         'type':'data',
    #         'text': json.dumps({"PH":ph,"TDS":tds,"TEMP":temp})
    #     }))


# AsyncWebsocketConsumer
class MyConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        self.groupname = 'dashboard'
        await self.channel_layer.group_add(
            self.groupname,
            self.channel_name
        )
        await self.accept()

    async def receive(self,text_data):
        try:
            result = json.loads(text_data)
            print('>>>>>>>>',result)
            ph = result['PH']
            await self.channel_layer.group_send({
            self.groupname,
                {   
                    'type':'deprocessing',
                    'value':ph
                }
            })
        except json.decoder.JSONDecodeError:
            print('The string does NOT contain valid JSON')        

    async def deprocessing(self,event):
        val_ph = event['value']
        self.send(text_data = json.dumps({'value':val_ph}))

    async def disconnect(self,close_code):
        # await self.disconnect()
        pass

# SyncConsumer
class MySyncConsumer(SyncConsumer):
    def websocket_connect(self,event):
        print('Websocket Connected....',event)
        self.send({
            'type':'websocket.accept',
        })
    def websocket_receive(self,event):
        # print('Receiving from client',event['text'])
        while True:
            self.send({
            'type':'websocket.send',
            'text': json.dumps({"PH":mqtt.PH,"TDS":mqtt.TDS,"TEMP":mqtt.TEMP}), 
            })
            sleep(3)
    def websocket_disconnect(self,event):
        raise StopConsumer()

# AsyncConsumer
class MyAsyncConsumer(AsyncConsumer):
    async def websocket_connect(self,event):
        print('Websocket Connected....',event)
        await self.send({
            'type':'websocket.accept',

        })
    async def websocket_receive(self,text_data):
        # while True:
        #     await self.send({
        #     'type':'websocket.send',
        #     'text': json.dumps({"PH":mqtt.data['PH'],"TDS":mqtt.data['TDS'],"TEMP":mqtt.data['TEMP']}), 
        #     })
        #     await asyncio.sleep(3)
        try:
            await self.send({
            'type':'websocket.send',
            # 'text': json.dumps({"PH":data['PH'],"TDS":data['TDS'],"TEMP":data['TEMP']}), 
            'text': text_data
            })
        except:
            print('The string does NOT contain valid JSON')


    async def websocket_disconnect(self,event):
        raise StopConsumer()