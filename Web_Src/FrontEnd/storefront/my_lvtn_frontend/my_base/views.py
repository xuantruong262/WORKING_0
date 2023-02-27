from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.

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