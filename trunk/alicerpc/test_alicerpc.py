#!/usr/bin/env python

import xmlrpclib
s = xmlrpclib.Server('http://alicerpc.appspot.com/xmlrpc/')
print s.alice.getInfo()
print s.alice.version()

#print s.app.respond("Hi")
while True: 
  print s.alice.respond(raw_input(">>> "))
