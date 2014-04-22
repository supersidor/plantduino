from django.conf.urls import patterns, include, url

from django.contrib import admin
from django.views.generic import RedirectView


admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'plant_web.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),
    #url(r'^plant/', include('plant.urls')),
    #(r'^favicon\.ico$', 'django.views.generic.simple.redirect_to', {'url': '/home/pi/plantduino/web/lemon.ico'}),
    (r'^favicon\.ico$', RedirectView.as_view(url='/static/lemon.ico')),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^accounts/login/$', 'django.contrib.auth.views.login'),                       
    url(r'', include('plant.urls')),
)
