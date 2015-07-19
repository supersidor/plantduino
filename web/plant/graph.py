import random
import django
import datetime
import matplotlib
matplotlib.use('Agg')

from django.http import HttpResponse    
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.dates import DateFormatter,HourLocator,AutoDateFormatter,date2num
from models import Sensor,Value
import matplotlib.pyplot as plt
from django.utils import timezone
import pytz
import time
import scipy.interpolate as sp
import numpy


from django.utils.timezone import get_current_timezone

CurrentTZ = get_current_timezone()
def simple(request,sensor):
    print CurrentTZ
    fig, ax = plt.subplots()
    plt.tight_layout()
    x=[]
    y=[]
    now = timezone.now()
    print 'now(UTC)',now
    print 'is_aware',timezone.is_aware(now)
    #now = datetime.datetime(2014, 07, 19, 2, 56)
    #print now
    yeasterday = now - datetime.timedelta(days=1)
    start_time = time.time()
    result = Value.objects.filter(pub_date__gte=yeasterday).filter(sensor__name=sensor)
    max = None
    min = None
    for e in result:
        if (max is None) or (max<e.value):
            max = e.value
        if (min is None) or (min>e.value):
            min = e.value    
    #yeasterday+datetime.timedelta(hours=i)        
    
    print "DB %d ms " % (1000*(time.time() - start_time))

    start_time = time.time()
    x_time = []
    i = 0
    for e in result:
        i = i + 1
        #if i % 10 != 1:
        #    continue
        #print 'from db',e.pub_datea
        local =  timezone.localtime(e.pub_date)
        #print 'local',local
        x.append(local)
        y.append(e.value)
    #x_time = date2num(x)
    #print x_time
    print "convertation %d ms " % (1000*(time.time() - start_time))

    print "DB %d records " % (len(x))

    #fc = sp.interp1d(x_time, y,kind='slinear')

    start_time = time.time()
    ax.plot_date(x, y, '-',tz=CurrentTZ)
    #ax.plot_date(x, y, '-')

    ax.xaxis.set_major_formatter(DateFormatter('%H',tz=CurrentTZ))
    #ax.xaxis.set_major_formatter(DateFormatter('%H'))

    ax.xaxis.set_major_locator(HourLocator(interval=1,tz=CurrentTZ))
    ax.tick_params(labeltop=True, labelright=True)
    fig.autofmt_xdate()


    plt.grid(True)

    ax.text(0.05, 0.9,"current: %.2f min:%.2f max:%.2f" % (y[-1],min,max),
            transform = ax.transAxes,color='green',fontsize=13)

    canvas=FigureCanvas(fig)
    response=django.http.HttpResponse(content_type='image/png')
    canvas.print_png(response)
    print "garph building %d ms " % (1000*(time.time() - start_time))

    return response
    #return HttpResponse(str(min)+"--"+str(max))
