#!/usr/bin/python
from SimpleXMLRPCServer import SimpleXMLRPCServer
import httplib, urllib
import serial
import sys
import datetime

import time
from threading import Thread
from threading import RLock
import os
import time

def sendToDb(sensors):
	ts = int(time.time())
	for sensor in sensors:
		value = sensors[sensor]
		conn = httplib.HTTPConnection("127.0.0.1")

		request = "/sensor?sensor=%s&value=%s&time=%d" % (sensor,value,ts)
		#print request
		conn.request("GET", request)
		response = conn.getresponse()

		#print "django response:",response.status, response.reason
		data = response.read()

		conn.getresponse
		conn.close()

def sensorsLoop(plant):
	while True:
		sensors = plant.getSensors()
                try:
			print "arduino time:"+plant.getTime().strftime('%Y-%m-%d %H:%M:%S')
                        sendToDb(sensors)
                except Exception as inst:
                        print inst#
#		try:
#			
#			sendToCosm(sensors)
#		except Exception as inst:
#			print inst			

 		time.sleep(5*60)
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
    logPrefix = "!LOG!"
    def __init__(self,device):
        self.conn = None;
        self.device = device;
	self.lock = RLock()
    def startLogThread(self):
	    thread = Thread(target = self.logLoop)
	    thread.daemon = True
	    thread.start()
    def logLoop(self):
        conn = self.getConn()
        while True:
		with self.lock:
			if conn.inWaiting()>0:
				line = conn.readline()
				line = line.rstrip()
				if line.startswith(Plant.logPrefix):
					self.processLog(line[len(Plant.logPrefix):])
				else:
					print "ERROR:INVALID TEXT RECEIVED:"+line
                time.sleep(2)

    def processLog(self,message):
        print "log message:"+message
		
    def sendCommand(self,command,log,*arg):
        with self.lock:
		return self._sendCommand(command,log,*arg)
    def _sendCommand(self,command,log,*arg):
        conn = self.getConn()
	if log:
		print "%s: sending %s %s" % (str(datetime.datetime.now()),command,str(arg)),
        conn.write(command+"\n")	
	if log:
		print ";sent"
        for a in arg:
            conn.write(a+"\n")    
	conn.flush()
	result = []
	while True:
		line = conn.readline()
		line = line.rstrip()
		if len(line)==0:
			break
		if line.startswith(Plant.logPrefix):
			self.processLog(line[len(Plant.logPrefix):])
		else:
			result.append(line)
	if log:
		print "%s got results: %s" % (str(datetime.datetime.now()),result)
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
    	cmd_result = self.sendCommand("state",True)
    	result = {}
    	for line in cmd_result:
    		parts = line.split(':')
    		result[parts[0]] = float(parts[1]) 
        return result
    def getTime(self):
        cmd_result = self.sendCommand("get_time",True)
        result = {}
        stime = cmd_result[0];
        return datetime.datetime.utcfromtimestamp(int(stime))
    def syncTime(self):
        self.sendCommand("set_time",True,"%d" % (time.time()+2*60*60))
    def getLight(self):
	    sensors = self.getSensors()
	    return sensors['light']!=0.0
    def setLight(self,newState):
        self.sendCommand("light",True,"1" if newState else "0")
    def resetLight(self):
        self.sendCommand("reset_light",TRue)
    def water(self):
        self.sendCommand("water",True)

plant = Plant(sys.argv[1])
server = SimpleXMLRPCServer(("0.0.0.0", 8888),allow_none=True)
server.register_instance(plant)
print "Server started"
print "time:"+plant.getTime().strftime('%Y-%m-%d %H:%M:%S')
#time.sleep(5)
#plant.resetLight()
plant.syncTime()
print plant.getSensors()

thread = Thread(target = sensorsLoop, args = (plant, ))
thread.daemon = True
thread.start()

plant.startLogThread()

server.serve_forever()




