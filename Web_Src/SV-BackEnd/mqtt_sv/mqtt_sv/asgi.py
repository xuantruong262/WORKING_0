"""
ASGI config for mqtt_sv project.

It exposes the ASGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/4.1/howto/deployment/asgi/
"""

import os
from django.core.asgi import get_asgi_application
from channels.routing import ProtocolTypeRouter, URLRouter
from channels.auth import AuthMiddlewareStack
from django.urls import path
import mqtt_fe.routing


os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'mqtt_sv.settings')

application = ProtocolTypeRouter({
    'http' : get_asgi_application(),
    # 'websocket':URLRouter(
    #        mqtt_fe.routing.websocket_urlpatterns
    #     )
    'websocket':AuthMiddlewareStack(URLRouter(mqtt_fe.routing.websocket_urlpatterns))
})
