#!/usr/bin/env python

import aiml
import os.path

k = aiml.Kernel()

k.setPredicate("secure", "yes") # secure the global session
k.bootstrap(learnFiles="std-startup.xml", commands="bootstrap")
k.setPredicate("secure", "no") # and unsecure it.
k.saveBrain("standard.brn")

