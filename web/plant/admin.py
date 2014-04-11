from django.contrib import admin


from plant.models import Sensor
from plant.models import Value

admin.site.register(Sensor)
admin.site.register(Value)
