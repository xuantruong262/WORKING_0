from django.template import Template,Context
from django.shortcuts import render
from django.http import HttpResponse
from django.http import HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt

# Create your views here.

active_path = "TRUONG IS HERE"
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

def home(request):
    return HttpResponse('THIS IS HOME PAGE')
device_id = ""
device_name = ""
def test_func(request):
    device_id = request.POST.get('device_id',False)
    device_name = request.POST.get('device',False)
    if device_id == "123": 
        return HttpResponseRedirect('device')
    else:
        return HttpResponseRedirect('chart')