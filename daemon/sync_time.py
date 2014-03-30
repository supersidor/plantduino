#!/usr/bin/python
import xmlrpclib
s = xmlrpclib.ServerProxy('http://192.168.1.205:8000',use_datetime=True)
s.syncTime()
print "new time: %s" % s.getTime().strftime('%Y-%m-%d %H:%M:%S')

