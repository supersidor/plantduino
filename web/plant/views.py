from django.shortcuts import render
from django.http import HttpResponse

from django.template import RequestContext, loader
import datetime
from models import Sensor,Value
from django.contrib.auth.decorators import login_required
from django.shortcuts import redirect
import time
from plant_proxy import Plant
from tzlocal import get_localzone

@login_required
def index(request):
    s = Plant().getProxy()
    if request.method=='POST':
        if 'turn_light' in request.POST:
            state = request.POST['turn_light']
            if state=="1":
                s.setLight(True)
            else:
                s.setLight(False)
        if 'turn_water' in request.POST:
            s.water()

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
    from django.utils.timezone import get_current_timezone
    from django.utils import timezone
    if (len(time)==0):
        time_p = timezone.now()
    else:
        time_p = datetime.datetime.fromtimestamp(int(time),tz=get_current_timezone())
    
    objs = Sensor.objects.filter(name=sensor)
    sensor = None
    if len(objs)>0:
        sensor = objs[0]
    else:
        sensor = Sensor(name=sensor)
        sensor.save()

    try:
        latest = Value.objects.filter(sensor=sensor).order_by('-pub_date')[0]
        #print "latest",str(latest)
        if time_p<latest.pub_date:
           print "WARNNING: Sensor time should be greater than last in DB"
        if latest.value==float(value):
            result = "No need to update data for sensor %s" % sensor
            print result
            HttpResponse(result)
    except IndexError:
        pass

    v = Value(sensor=sensor,pub_date=time_p,value=float(value))
    v.save()
    return HttpResponse(str(sensor.name)+":"+str(value))

@login_required
def data(request,sensor):
    template = loader.get_template('plant/data.html')
    context = RequestContext(request, {
        'sensor': sensor,
    })
    return HttpResponse(template.render(context))
@login_required
def index2(request):
    s = Plant().getProxy() 
    if request.method=='POST':
        state = request.POST['turn_light']
        if state=="1":
            s.setLight(True)
        else:
            s.setLight(False)

    template = loader.get_template('plant/index2.html')
    d = s.getSensors()
    turn_on_light = False
    if int(d['light'])==0:
        turn_on_light = True

    context = RequestContext(request, {
        'sensors': d,
        'turn_on_light': turn_on_light
    })
    
    return HttpResponse(template.render(context))

