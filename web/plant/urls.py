from django.conf.urls import patterns, url
from plant import views,graph

urlpatterns = patterns('',
    url(r'^sensor', views.sensor, name='sensor'),
    url(r'^light', views.light, name='light'),
    url(r'^data/(?P<sensor>[\w]+)', views.data, name='data'),    
    url(r'^graph/(?P<sensor>[\w]+)', graph.simple, name='simple'),    
    url(r'^$', views.index, name='index')

)
