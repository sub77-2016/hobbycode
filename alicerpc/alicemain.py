#!/usr/bin/env python

import cgi
import logging  
import os

from google.appengine.api import users
from google.appengine.ext import webapp                                        
from google.appengine.ext.webapp.util import run_wsgi_app           
from google.appengine.ext.webapp import template       
from google.appengine.runtime import DeadlineExceededError                                                           
     
from StringIO import StringIO                                                  
import traceback
import xmlrpclib
from xmlrpcserver import XmlRpcServer

import aiml
                                                                               

class AliceBot:
    def __init__(self):
        self.aiml_k = aiml.Kernel()
        if os.path.isfile("standard.brn"):
	  self.aiml_k.verbose(False)
          try:
            self.aiml_k.bootstrap(brainFile = "standard.brn")    
          except DeadlineExceededError:
            self.response.clear()
            self.response.set_status(500)
            self.response.out.write("bootstrap() could not be completed in time...") 
	  #self.aiml_k.verbose(True) 
       
    def respond(self,meta,inputrpc):                                                    
        return self.aiml_k.respond(input=inputrpc)

    def version(self,meta):                                                    
	return self.aiml_k.version()
 
    def getInfo(self,meta):                                                    
        return 'AIML - RPC'

# The Main Global Chatterbot
alice = AliceBot()

class ChatHandler(webapp.RequestHandler):
  def __init__(self):
    pass

  def get(self):
    #greetings_query = Greeting.all().order('-date')
    #greetings = greetings_query.fetch(10)

    if users.get_current_user():
      url = users.create_logout_url(self.request.uri)
      url_linktext = 'Logout'
    else:
      url = users.create_login_url(self.request.uri)
      url_linktext = 'Login'

    user_input = cgi.escape(self.request.get('input'))
    bot_output = alice.aiml_k.respond(user_input)

    template_values = {
      'user_input': user_input,
      'bot_output': bot_output,
      'url_linktext': url_linktext,
      }

    path = os.path.join(os.path.dirname(__file__), 'index.html')
    self.response.out.write(template.render(path, template_values))

  def post(self):
    if users.get_current_user():
      url = users.create_logout_url(self.request.uri)
      url_linktext = 'Logout'
    else:
      url = users.create_login_url(self.request.uri)
      url_linktext = 'Login'

    input = cgi.escape(self.request.get('input'))
    output = alice.aiml_k.respond(input)

    template_values = {
      'input': 'input',
      'output': 'output',
      'url_linktext': 'url_linktext',
      }

    path = os.path.join(os.path.dirname(__file__), 'index.html')
    #self.response.out.write(template.render(path, template_values)


class XMLRpcHandler(webapp.RequestHandler):                                    
    rpcserver = None
                                
    def __init__(self): 
        webapp.RequestHandler.__init__(self)        
        self.rpcserver = XmlRpcServer()                                        
        #alice = AliceBot()                                                    
        self.rpcserver.register_class('alice',alice)                               
                                                                               
    def post(self):
        request = StringIO(self.request.body)
        request.seek(0)                                                        
        response = StringIO()                                                  
        try:
            self.rpcserver.execute(request, response, None)                    
        except Exception, e:                                                   
            logging.error('Error executing: '+str(e))                          
            for line in traceback.format_exc().split('\n'):                    
                logging.error(line)
        finally:
            response.seek(0)  
                                                                               
        rstr = response.read()                                                 
        self.response.headers['Content-type'] = 'text/xml'                     
        self.response.headers['Content-length'] = "%d"%len(rstr)               
        self.response.out.write(rstr)
 
                                                                                                          
def main():
  application = webapp.WSGIApplication([('/', ChatHandler),
                                        ('/xmlrpc/', XMLRpcHandler)],
                                        debug=True)   
  run_wsgi_app(application)                                                    
 
       
if __name__ == "__main__":
    main()

