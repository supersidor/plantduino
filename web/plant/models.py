from django.db import models

# Create your models here.
from django.db import models


class Sensor(models.Model):
    name = models.CharField(max_length=50)

class Value(models.Model):
    sensor = models.ForeignKey(Sensor)
    pub_date = models.DateTimeField()
    value = models.DecimalField(max_digits=10, decimal_places=2)

