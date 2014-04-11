import os
os.environ['PYTHONPATH'] = '/home/pi/plantduino/web'
os.environ['DJANGO_SETTINGS_MODULE'] = 'plant_web.settings'
from plant.models import Sensor,Value
