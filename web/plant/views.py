from django.shortcuts import render
from django.http import HttpResponse
import xmlrpclib
from django.template import RequestContext, loader
import datetime
from models import Sensor,Value
from django.contrib.auth.decorators import login_required
from django.shortcuts import redirect
import time
@login_required
def index(request):
    s = xmlrpclib.ServerProxy('http://127.0.0.1:8000')
    
    if request.method=='POST':
        state = request.POST['turn_light']
        if state=="1":
            s.setLight(True)
        else:
            s.setLight(False)
#        time.sleep(1)

    template = loader.get_template('plant/index.html')
    d = s.getSensors()
    turn_on_light = False
    if int(d['light'])==0:
        turn_on_light = True

    context = RequestContext(request, {
        'sensors': d,
        'turn_on_light': turn_on_light
    })
    
    return HttpResponse(template.render(context))

@login_required
def light(request):
    state = request.POST['turn_light']
    s = xmlrpclib.ServerProxy('http://127.0.0.1:8000')
    if state=="1":
        s.setLight(True)
    else:
        s.setLight(False)
    return redirect("/")

def sensor(request):
    sensor = request.GET.get('sensor', '')
    value = request.GET.get('value','')
    time = request.GET.get('time','')
    time_p = datetime.datetime.fromtimestamp(int(time))
    objs = Sensor.objects.filter(name=sensor)
    if len(objs)>0:
        obj = objs[0]
    else:
        obj = Sensor(name=sensor)
        obj.save()
    v = Value(sensor=obj,pub_date=time_p,value=float(value))
    v.save()
    return HttpResponse(str(obj.id)+":"+str(v.id))
