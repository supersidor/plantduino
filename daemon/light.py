#!/usr/bin/python
import xmlrpclib
s = xmlrpclib.ServerProxy('http://192.168.1.205:8000')
print s.getSensors()
s.setLight(not s.getLight())
