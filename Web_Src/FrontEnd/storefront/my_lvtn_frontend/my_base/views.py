from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.

def base(request):
    return render(request,'home/base.html')


def dashboard(request):
    return render(request,'home/dashboard.html')

def home(request):
    return HttpResponse('THIS IS HOME PAGE')