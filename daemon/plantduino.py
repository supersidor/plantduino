from SimpleXMLRPCServer import SimpleXMLRPCServer
import httplib, urllib
import serial
import sys

import time
from threading import Thread
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
    def close(self):
        if self.conn:
            self.conn.close()
    def getConn(self):
        if not self.conn:
            print sys.argv[1]
            self.conn = serial.Serial(self.device, 9600)
            print self.conn.readline(),

        return self.conn;
    def getSensors(self):
        conn = self.getConn()
        conn.write("state\n")
        conn.flush()
        while True:
            print conn.readline(),
        return {'air_temp':20,'humidity':30}
plant = Plant(sys.argv[1])
#server = SimpleXMLRPCServer(("0.0.0.0", 8000))
#server.register_instance(plant)
#thread = Thread(target = cosmLoop, args = (plant, ))
#thread.daemon = True
#thread.start()
#print "Server started"
#server.serve_forever()
print plant.getSensors()



