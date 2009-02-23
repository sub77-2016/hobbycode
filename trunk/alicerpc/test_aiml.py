#!/usr/bin/env python

import aiml
import os.path

k = aiml.Kernel()
if os.path.isfile("standard.brn"):
    k.bootstrap(brainFile = "standard.brn")
else:
    k.setPredicate("secure", "yes") # secure the global session
    k.bootstrap(learnFiles="std-startup.xml", commands="bootstrap")
    k.setPredicate("secure", "no") # and unsecure it.
    k.saveBrain("standard.brn")

while True: print k.respond(raw_input("> "))


