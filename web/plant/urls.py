from django.conf.urls import patterns, url
from plant import views

urlpatterns = patterns('',
    url(r'^sensor', views.sensor, name='sensor'),
    url(r'^light', views.light, name='light'),
    url(r'^$', views.index, name='index')
)
