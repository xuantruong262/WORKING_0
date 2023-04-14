from django.template import Template,Context
from django.shortcuts import render
from django.http import HttpResponse
from django.http import HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt
from mqtt_sv import mqtt
import json


def send_data(request):
    return render(request,'my_test.html')

def home(request):
    return render(request,'my_test.html')

def base(request):
    return render(request,'home/base.html')
def dashboard(request):
    return render(request,'home/dashboard.html')

def device(request):
    return render(request,'home/device.html')

def chart(request):
  
    return render(request,'home/chart.html')

def about(request):
    return render(request,'home/about.html')


def setpoint_func(request):
    PH_setpoint = request.POST.get('ph',False)
    PH_THR_setpoint = request.POST.get('ph_thr',False)  
    TDS_setpoint = request.POST.get('tds',False)
    TDS_THR_setpoint = request.POST.get('tds_thr',False)
    print(PH_setpoint)
    print(PH_THR_setpoint)
    print(TDS_setpoint)
    print(TDS_THR_setpoint)
    global my_setpoint
    my_setpoint = json.dumps({
        "PH": PH_setpoint,
        "PH_THR":PH_THR_setpoint,
        "TDS":TDS_setpoint,
        "TDS_THR":TDS_THR_setpoint
    })
    mqtt.client.publish("XuanTruong/setpoint",my_setpoint,0,False)
    return HttpResponseRedirect('device')
    # if device_id == "123": 
    #     return HttpResponseRedirect('device')
    # else:
    #     return HttpResponseRedirect('chart')
# Create your views here.
