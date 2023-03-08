import json
from channels.consumer import SyncConsumer,AsyncConsumer
from channels.exceptions import StopConsumer
from channels.generic.websocket import WebsocketConsumer
from time import sleep
import asyncio

class MyConsumer(WebsocketConsumer):
    def connect(self):
        self.accept()

        self.send(text_data=json.dumps({
            'type': 'connect_establish',
            'message': 'I am connecting'
        }))

# class MySyncConsumer(SyncConsumer):
#     def websocket_connect(self,event):
#         print('Websocket Connected....',event)
#         self.send({
#             'type':'websocket.accept',

#         })
#     def websocket_receive(self,event):
#         print('Receiving from client',event['text'])
#         for i in range(10):
#             self.send({
#             'type':'websocket.send',
#             'text':str(i)
#         })
#         sleep(1)
#     def websocket_disconnect(self,event):
#         raise StopConsumer()
    
# class MyAsyncConsumer(AsyncConsumer):
#     async def websocket_connect(self,event):
#         print('Websocket Connected....',event)
#         await self.send({
#             'type':'websocket.accept',

#         })
#     async def websocket_receive(self,event):
#         for i in range(10):
#             await self.send({
#             'type':'websocket.send',
#             'text':str(i)
#         })
#         await asyncio.sleep(1)
#     async def websocket_disconnect(self,event):
#         raise StopConsumer()