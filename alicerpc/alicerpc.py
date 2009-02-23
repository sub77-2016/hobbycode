from google.appengine.ext import webapp                                        
from google.appengine.ext.webapp.util import run_wsgi_app                      
import logging                                                                 
     
from StringIO import StringIO                                                  
import traceback
import xmlrpclib
from xmlrpcserver import XmlRpcServer

import aiml
import os.path
                                                                               

class Application:
    def __init__(self):
        self.aiml_k = aiml.Kernel()
        if os.path.isfile("standard.brn"):
	  self.aiml_k.verbose(False)
          self.aiml_k.bootstrap(brainFile = "standard.brn")            
       
    def respond(self,meta,inputrpc):                                                    
        #print(self.aiml_k.respond(input=inputrpc))
	return self.aiml_k.version()
 
    def getName(self,meta):                                                    
        return 'example - rpc'

        
class XMLRpcHandler(webapp.RequestHandler):                                    
    rpcserver = None
                                
    def __init__(self):         
        self.rpcserver = XmlRpcServer()                                        
        app = Application()                                                    
        self.rpcserver.register_class('app',app)                               
                                                                               
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
  application = webapp.WSGIApplication([('/xmlrpc/', XMLRpcHandler)],
                                     debug=True)   
  run_wsgi_app(application)                                                    
        
if __name__ == "__main__":
    main()

