import xmlrpclib
import os

class DumbPlant:
	def getSensors(self):
		return {'air_temp':25.55, 'temp_board':28.55, 'humidity':42.55, 'illumination':188.55, 'light':1.00, 'force_light':0.00, 'force_light_state':0.00, 'soil_temp':27.55, 'soil_moisture':27.55}

class Plant:
	def getProxy(self):
		if os.getenv('DUMB_PLANT', 'FALSE')=='TRUE':
			return DumbPlant()
		else:
			return xmlrpclib.ServerProxy('http://127.0.0.1:8000')


