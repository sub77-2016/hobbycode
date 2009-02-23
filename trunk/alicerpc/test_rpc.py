#!/usr/bin/env python

import xmlrpclib
s = xmlrpclib.Server('http://localhost:8080/xmlrpc/')
print s.app.getName()

print s.app.respond("Hi")
