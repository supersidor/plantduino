from django.shortcuts import render
from django.http import HttpResponse
import xmlrpclib
from django.template import RequestContext, loader

def index(request):
    s = xmlrpclib.ServerProxy('http://192.168.1.207:8000')
    #temp = "<head></head><body>%s</body>"
    #return HttpResponse(temp % str(s.getSensors()))
    template = loader.get_template('plant/index.html')
    d = s.getSensors()
    context = RequestContext(request, {
        'sensors': d
    })
    return HttpResponse(template.render(context))

