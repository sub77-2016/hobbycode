#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# ichatbot - MSN chat robot by Hui Chen <usa.chen [at] gmail.com>
# Released under GPL v3
#
# Check out instructions and open tickets in project homepage: 
#	http://code.google.com/p/ichatbot/
#
# Thanks to: 
#	1. Alberto Bertogli for msnlib
#	2. Cort Stratton for PyAIML

import sys
import os
import time
import select
import socket
import thread
import msnlib
import msncb
import re
from datetime import datetime
import aiml
import marshal

loginemail="mybot@email.address" # MSN account that robot will login to
loginpassword="msnpassword" # account password
botname="mychatbot" # Robot's MSN nickname
admin_email="mymsn@email.com" # emailaddress for admin the robot

# ==== normally nothing needs to be changed below ====

rootdir=os.getcwd()
configfile="%s/chatbot.ini" % rootdir
sessiondir="%s/sessions/%s" % (rootdir,loginemail)
os.system("mkdir -p %s" % sessiondir)
logfilename="%s/log" % sessiondir 
sessionfilename="%s/session" % sessiondir

os.system("mkdir -p %s/tmp/" % rootdir)
msn_fortune="%s/tmp/msn-fortune" % rootdir
msn_fortune_zh="%s/tmp/msn-fortune-zh" % rootdir

blacklist=['sample@blacklist.org' ] # put here email addresses you want to block
blacklist=set(blacklist)
adminlist=[admin_email] # the 'admin' account's email address from which you send admin command
adminlist=set(adminlist)

dlist=[]
talkedto=[]

use_brain=True

def fortune(): # tell a fortune cookie in English
	u'''fc\tTell a fortune cookie'''
	global msn_fortune
	os.system("fortune fortunes literature riddles > %s" % msn_fortune)
	f=open(msn_fortune)
	message=f.read()
	f.close()
	message=getheader()+message+getfooter()
	return message

def fortunezh(): # tell a Chinese poem
	u'''sc\t背诵一首唐诗或者宋词'''
	global msn_fortune_zh
	os.system("fortune-zh > %s" % msn_fortune_zh)
	f=open(msn_fortune_zh)
	message=f.read()
	f.close()
	message=message.replace('\x1b[32m','')
	message=message.replace('\x1b[33m','')
	message=message.replace('\x1b[m','')
	message=getheader()+message+getfooter()
	return message

def help():
	global doclist
	message=u'''Commands:

'''
	message=message+doclist
	message=getheader()+message+getfooter()
	return message.encode('UTF-8')

clist=[]
clist=[fortune, fortunezh]
doclist=u''

for i in clist:
	doclist=doclist+i.__doc__+'\n'


def config(configfile, k):
	# setup bot properties such name, birthplace etc
	f=open(configfile)
	g=f.readlines()
	f.close()
	for i in g:
		h=i.split('=')
		if len(h)==2:
			k.setBotPredicate(h[0].strip(), h[1].strip()) 

	# load session
	if os.path.exists(sessionfilename):
		f=open(sessionfilename)
		g=f.read()
		f.close()
		g=g.split('\n')
		for i in g:
			if i!='':
				talkedto.append(i)
				sessionFile = file("%s/%s.ses" %(sessiondir, i), "rb")
				session = marshal.load(sessionFile)
				sessionFile.close()
				for pred,value in session.items():
					k.setPredicate(pred, value, i)
    
			
# ================== Initialization
k = aiml.Kernel()
config(configfile, k)
k.verbose(False)

# ================== load brain 
if use_brain and os.path.isfile("chatbot.brn"):
    k.bootstrap(brainFile = "chatbot.brn")
else:
    k.bootstrap(learnFiles = "startup.xml", commands = "load aiml b")
    k.saveBrain("chatbot.brn")

def getheader():
	if dlist.has_key('User-Agent') and len(dlist['User-Agent'])>6 and dlist['User-Agent'][0:6]=='pidgin': 
		return "MIME-Version: 1.0\r\nX-MMS-IM-Format: FN=Microsoft%20Sans%20Serif; EF=B; CO=800080; CS=86; PF=22\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n\r\n"
	else:
		return "MIME-Version: 1.0\r\nX-MMS-IM-Format: FN=Microsoft%20Sans%20Serif; EF=B; CO=800080; CS=86; PF=22\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n"

def getfooter():
	return ''


if os.path.exists(logfilename):
	logfile=open(logfilename, "a")
else:
	logfile=open(logfilename, "w")

def dolog(message):
	global logfile
	logfile.write(message)
	logfile.flush()

def filter(message):
	message=message.replace("ALICE", botname)
	return message

def mycb_msg(md, type, tid, params, sbd):
	global dlist
	if type!="MSG":
		return
	sender=tid.split()
	senderemail=sender[0]
	if senderemail in blacklist:
		return	
	plist=params.split('\r\n')
	dlist=[]
	for i in plist:
		if i!='':
			r=i.split(': ')
			if len(r)==2:
				dlist.append(r)
			elif len(r)==1:
				dlist.append(['Message', r[0]])
	dlist=dict(dlist)

	if dlist.has_key('Message'):
		if not senderemail in set(talkedto):
			talkedto.append(senderemail)
		message=dlist['Message']
		logtime='%s/%s/%s %s:%s:%s' %( datetime.now().month, datetime.now().year, datetime.now().day, datetime.now().hour, datetime.now().minute, datetime.now().second)
		if not senderemail in adminlist:
			dolog('=== %s === Recv from %s <%s> ===\n%s\n' % (logtime, sender[1], senderemail, message))
		if message=='admquit' and senderemail in adminlist:
			message="%s is offline!" % botname
			message=getheader()+message+getfooter()
			m.sendmsg(senderemail, message)
			quit()
		if message=='admlist' and senderemail in adminlist:
			message=''
			m.sync()
			for i in m.users.keys():
				if m.users[i].status!="FLN":
					message= message+ "%s <%s> is %s\n" % (m.users[i].realnick, i, msnlib.reverse_status[m.users[i].status])
			message=getheader()+message+getfooter()
			m.sendmsg(senderemail, message)
			return

#		print 'Received message: %s' % message
#		print 'dlist=',dlist
#		print ''

		command2=message[0:2]
		command3=message[0:3]
		command4=message[0:4]

		if command4=="help":
			remsg=help()
		elif command2=='sc':	
			remsg=fortunezh()
		elif command2=='fc':
			remsg=fortune()
		else:
			if k.getPredicate("name", senderemail) =='':
				dispname=senderemail.split("@")
				if len(dispname) ==2:
					k.setPredicate("name", dispname[0], senderemail)
			remsg=k.respond(message, senderemail)
			remsg=filter(remsg)
			remsg=getheader() + remsg+getfooter()
		m.sendmsg(senderemail, remsg)
		remsg=remsg.split('\r\n\r\n')
		logtime='%s/%s/%s %s:%s:%s' %( datetime.now().month, datetime.now().year, datetime.now().day, datetime.now().hour, datetime.now().minute, datetime.now().second)
		if not senderemail in adminlist:
			dolog('=== %s === Send to %s <%s>===\n%s\n\n' % (logtime, sender[1], senderemail, remsg[1]))
		
m = msnlib.msnd()
m.cb = msncb.cb()
m.cb.msg=mycb_msg

m.email = loginemail
m.pwd = loginpassword 
m.encoding="UTF-8"

m.login()
m.sync()
m.change_status("online")
m.change_nick(botname.encode('UTF-8'))
m.privacy(1, 0)

print "Login successful"

def savesession():
	# =========== save session
	f=open(sessionfilename, 'w')
	for i in talkedto:
		session = k.getSessionData(i)
		sessionFile = file("%s/%s.ses" % (sessiondir, i), "wb")
		marshal.dump(session, sessionFile)
		sessionFile.close()
		f.write("%s\n"% i)
	f.close()


def quit():
	global logfile
	logfile.close()
	savesession() # save session data before quiting
	try:
		m.disconnect()
	except:
		pass
	print "Exiting."
	sys.exit(0)

def do_work():
	time.sleep(10*60)
	savesession() # save session data every 10 min

thread.start_new_thread(do_work, ())

while 1:
	t = m.pollable()
	infd = t[0]
	outfd = t[1]

	try:
		fds = select.select(infd, outfd, [], 0)
	except:
		quit()
	
	for i in fds[0] + fds[1]:       # see msnlib.msnd.pollable.__doc__
		try:
			m.read(i)
		except ('SocketError', socket.error), err:
			if i != m:
				m.close(i)
			else:
				quit()
	time.sleep(0.01)

