
from django.contrib import admin
from django.urls import path
from . import publisher

urlpatterns = [
    path('admin/', admin.site.urls),
    path('publish', publisher.publish_message, name='publish')
]
