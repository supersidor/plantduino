from SimpleXMLRPCServer import SimpleXMLRPCServer
import httplib, urllib
import serial
import sys

import time
from threading import Thread
from threading import RLock

def cosmLoop(plant):
	while True:
		try:
			sendToCosm(plant.getSensors())
		except Exception as inst:
			print inst			
 		time.sleep(60)
def sendToCosm(sensors):
    headers = {"X-PachubeApiKey": "cvv0iZ8JCOr7bPZs9g3p6jXCqz2mg2tsjLfUw69hqo8",
            "Host": "www.pachube.com",
                        "Content-Type":"text/csv",
                        "Connection":"close"}
    conn = httplib.HTTPConnection("www.pachube.com:80")
    param_order = ['air_temp','soil_temp','humidity','soil_moisture','water_temp','soil_moisture_aggr','pressure','illumination']
    request = ""
    for param in param_order:
        if len(request)>0:
            request = request + ","
        if sensors.has_key(param):
            request = request + str(sensors[param])
    print "send to cosm:",request
    conn.request("PUT", "/api/45931.csv",request, headers)
    response = conn.getresponse()
    print "cosm response:",response.status, response.reason
    data = response.read()
    conn.close()

class Plant:
    def __init__(self,device):
        self.conn = None;
        self.device = device;
	self.lock = RLock()
    def sendCommand(self,command):
        with self.lock:
		return self._sendCommand(command)
    def _sendCommand(self,command):
        conn = self.getConn()
        conn.write(command+"\n")	    
	conn.flush()
	result = []
	while True:
		line = conn.readline()
		line = line.rstrip()
		if len(line)==0:
			break
		result.append(line)
	return result
    def close(self):
        if self.conn:
            self.conn.close()
    def getConn(self):
        if not self.conn:
            print "Connecting to "+self.device
            self.conn = serial.Serial(self.device, 9600)
            print self.conn.readline(),
        return self.conn;
    def getSensors(self):
	cmd_result = self.sendCommand("state")
	result = {}
	for line in cmd_result:
		parts = line.split(':')
		result[parts[0]] = float(parts[1]) 
        return result
plant = Plant(sys.argv[1])
server = SimpleXMLRPCServer(("0.0.0.0", 8000))
server.register_instance(plant)
thread = Thread(target = cosmLoop, args = (plant, ))
thread.daemon = True
thread.start()
print "Server started"
print plant.getSensors()
server.serve_forever()




