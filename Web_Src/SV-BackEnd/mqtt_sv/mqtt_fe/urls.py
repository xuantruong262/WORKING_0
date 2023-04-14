
from django.urls import path
from . import views
from django.contrib.staticfiles.urls import staticfiles_urlpatterns
urlpatterns = [

    path('', views.home,name="home"),
    path('base/setpoint', views.setpoint_func, name = "setpoint"),
    path('base', views.base, name= "base"),
    path('base/dashboard',views.dashboard,name= "dashboard"),
    path('base/device',views.device,name = "device"),
    path('base/device/123',views.device,name = "device_123"),
    path('base/chart',views.chart,name = "chart"),
    path('base/about',views.about,name = "about"),
]


urlpatterns += staticfiles_urlpatterns()