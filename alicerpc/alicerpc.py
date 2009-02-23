#!/usr/bin/env python

import cgi

from google.appengine.api import users
from google.appengine.ext import webapp                                        
from google.appengine.ext.webapp.util import run_wsgi_app                      
import logging                                                                 
     
from StringIO import StringIO                                                  
import traceback
import xmlrpclib
from xmlrpcserver import XmlRpcServer

import aiml
import os
                                                                               

class Application:
    def __init__(self):
        self.aiml_k = aiml.Kernel()
        if os.path.isfile("standard.brn"):
	  self.aiml_k.verbose(False)
          self.aiml_k.bootstrap(brainFile = "standard.brn")    
	  self.aiml_k.verbose(True)        
       
    def respond(self,meta,inputrpc):                                                    
        return self.aiml_k.respond(input=inputrpc)

    def version(self,meta):                                                    
	return self.aiml_k.version()
 
    def getInfo(self,meta):                                                    
        return 'AIML - RPC'

class MainPage(webapp.RequestHandler):
  def get(self):
    self.response.out.write("""
      <html>
        <body>
          <form action="/sign" method="post">
            <div><textarea name="content" rows="3" cols="60"></textarea></div>
            <div><input type="submit" value="Sign Guestbook"></div>
          </form>
        </body>
      </html>""")


class Guestbook(webapp.RequestHandler):
  def post(self):
    self.response.out.write('<html><body>You wrote:<pre>')
    self.response.out.write(cgi.escape(self.request.get('content')))
    self.response.out.write('</pre></body></html>')
        
class XMLRpcHandler(webapp.RequestHandler):                                    
    rpcserver = None
                                
    def __init__(self): 
        webapp.RequestHandler.__init__(self)        
        self.rpcserver = XmlRpcServer()                                        
        alice = Application()                                                    
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
  application = webapp.WSGIApplication([('/xmlrpc/', XMLRpcHandler),
                                        ('/', MainPage),
                                        ('/sign', Guestbook)],
                                     debug=True)   
  run_wsgi_app(application)                                                    
        
if __name__ == "__main__":
    main()

