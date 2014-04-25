from django.db import models

# Create your models here.
from django.db import models


class Sensor(models.Model):
    name = models.CharField(max_length=50,db_index=True)
    def __str__(self):
    	return self.name

class Value(models.Model):
    sensor = models.ForeignKey(Sensor)
    pub_date = models.DateTimeField(db_index=True)
    value = models.DecimalField(max_digits=10, decimal_places=2)
    def __str__(self):
    	return str(self.sensor)+'::'+str(self.pub_date)+'::'+str(self.value)
