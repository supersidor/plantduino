import random
import django
import datetime
from django.http import HttpResponse    
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.dates import DateFormatter,HourLocator
from models import Sensor,Value
import matplotlib.pyplot as plt

def simple(request,sensor):
    fig, ax = plt.subplots()
    plt.tight_layout()
    x=[]
    y=[]
    now = datetime.datetime(2014, 04, 23, 9, 59)
    yeasterday = now - datetime.timedelta(days=1)
    result = Value.objects.filter(pub_date__gte='2014-04-22').filter(sensor__name=sensor)
    max = None
    min = None
    for e in result:
        if (max is None) or (max<e.value):
            max = e.value
        if (min is None) or (min>e.value):
            min = e.value    
    #yeasterday+datetime.timedelta(hours=i)        
    for e in result:
        x.append(e.pub_date)
        y.append(e.value)
    ax.plot_date(x, y, '-')
    ax.xaxis.set_major_formatter(DateFormatter('%H'))
    ax.xaxis.set_major_locator(HourLocator(interval=2))
    ax.tick_params(labeltop=True, labelright=True)
    fig.autofmt_xdate()
    plt.grid(True)

    canvas=FigureCanvas(fig)
    response=django.http.HttpResponse(content_type='image/png')
    canvas.print_png(response)
    return response
    #return HttpResponse(str(min)+"--"+str(max))
