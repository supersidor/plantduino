import xmlrpclib
import os

class DumbPlant:
	def __init__(self):
		self.state = {'air_temp':25.55, 'temp_board':28.55, 'humidity':42.55, 'illumination':188.55, 'light':1.00, 'force_light':0.00, 'force_light_state':0.00, 'soil_temp':27.55, 'soil_moisture':27.55}
	def getSensors(self):
		return self.state
	def setLight(self,light):
		if light:
			self.state['light'] = 1.0
		else:
			self.state['light'] = 0.0
	def water(self):
		self.state['soil_moisture'] = self.state['soil_moisture'] + 1

class Plant:
	def getProxy(self):
		if os.getenv('DUMB_PLANT', 'FALSE')=='TRUE':
			return DumbPlant()
		else:
			return xmlrpclib.ServerProxy('http://127.0.0.1:8888',use_datetime=True)


