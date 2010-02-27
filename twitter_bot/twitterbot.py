#!/usr/bin/python
"""
twitterbot

  A twitter IRC bot. Twitterbot connected to an IRC server and idles in
  a channel, polling a twitter account and broadcasting all updates to
  friends.
  
USAGE

  twitterbot [config_file]

CONFIG_FILE

  The config file is an ini-style file that must contain the following:
  
[irc]
server: <irc_server>
port: <irc_port>
nick: <irc_nickname>
channel: <irc_channel_to_join>

[twitter]
email: <twitter_account_email>
password: <twitter_account_password>

  If no config file is given "twitterbot.ini" will be used by default.
"""

BOT_VERSION = "TwitterBot 1.0 (http://mike.verdone.ca/twitter)"

import sys
import time
from dateutil.parser import parse
from ConfigParser import SafeConfigParser
from heapq import heappop, heappush
import traceback
import os.path

from api import Twitter, TwitterError
from util import htmlentitydecode

def debug(msg):
    # uncomment this for debug text stuff
    # print >> sys.stderr, msg
    pass

class SchedTask(object):
    def __init__(self, task, delta):
        self.task = task
        self.delta = delta
        self.next = time.time()

    def __repr__(self):
        return "<SchedTask %s next:%i delta:%i>" %(
            self.task.__name__, self.next, self.delta)
    
    def __cmp__(self, other):
        return cmp(self.next, other.next)
    
    def __call__(self):
        return self.task()

class Scheduler(object):
    def __init__(self, tasks):
        self.task_heap = []
        for task in tasks:
            heappush(self.task_heap, task)
    
    def next_task(self):
        now = time.time()
        task = heappop(self.task_heap)
        wait = task.next - now
        task.next = now + task.delta
        heappush(self.task_heap, task)
        if (wait > 0):
            time.sleep(wait)
        task()
        debug("tasks: " + str(self.task_heap))
        
    def run_forever(self):
        while True:
            self.next_task()

            
class TwitterBot(object):
    def __init__(self, configFilename):
        self.configFilename = configFilename
        self.config = load_config(self.configFilename)
        self.twitter = Twitter(
            self.config.get('twitter', 'email'),
            self.config.get('twitter', 'password'))
        self.sched = Scheduler(
            (SchedTask(self.process_events, 1),
             SchedTask(self.check_statuses, 120)))
        self.lastUpdate = time.gmtime()

    def check_statuses(self):
        debug("In check_statuses")
        try:
            updates = self.twitter.statuses.friends_timeline()
        except Exception, e:
            print >> sys.stderr, "Exception while querying twitter:"
            traceback.print_exc(file=sys.stderr)
            return
        
        nextLastUpdate = self.lastUpdate
        for update in updates:
            crt = parse(update['created_at']).utctimetuple()
            #print crt, self.lastUpdate 
            if (crt > self.lastUpdate):
                text = (htmlentitydecode(
                    update['text'].replace('\n', ' '))
                    .encode('utf-8', 'replace'))

                # Skip updates beginning with @
                # TODO This would be better if we only ignored messages
                #   to people who are not on our following list.

                if not text.startswith("@"):
                    print u"%s: %s" %(
                            update['user']['screen_name'],
                            text.decode('utf-8'))
                
                nextLastUpdate = crt
            else:
                break

        self.lastUpdate = nextLastUpdate
        
    def process_events(self):
        debug("In process_events")
                
    def follow(self, name):
        friends = [x['name'] for x in self.twitter.statuses.friends()]
        debug("Current friends: %s" %(friends))
        if (name in friends):
            print "=O_o= I'm already following %s." %(name)
        else:
            try:
                self.twitter.friendships.create(id=name)
            except TwitterError:
                print "=O_o= I can't follow that user. Are you sure the name is correct?"
                return
            print "^^\" Okay! I'm now following %s." %(name)
    
    def unfollow(self, name):
        friends = [x['name'] for x in self.twitter.statuses.friends()]
        debug("Current friends: %s" %(friends))
        if (name not in friends):
            print "=O_o= I'm not following %s." %(name)
        else:
            self.twitter.friendships.destroy(id=name)
            print "^^\" Okay! I've stopped following %s." %(name)
    
    def run(self):

        while True:
            try:
                self.sched.run_forever()
            except KeyboardInterrupt:
                break
            except TwitterError:
                # twitter.com is probably down because it sucks. ignore the fault and keep going
                pass

def load_config(filename):
    defaults = dict(server=dict(port=6667, nick="twitterbot"))
    cp = SafeConfigParser(defaults)
    cp.read((filename,))
    
    # attempt to read these properties-- they are required
    cp.get('twitter', 'email'),
    cp.get('twitter', 'password')
    #cp.get('irc', 'server')
    #cp.getint('irc', 'port')
    #cp.get('irc', 'nick')
    #cp.get('irc', 'channel')

    return cp

# So there was a joke here about the twitter business model
# but I got rid of it. Not because I want this codebase to
# be "professional" in any way, but because someone forked
# this and deleted the comment because they couldn't take
# a joke. Hi guy!
#
# Fact: The number one use of Google Code is to look for that
# comment in the Linux kernel that goes "FUCK me gently with
# a chainsaw." Pretty sure Linus himself wrote it.

def main():
    configFilename = "twitterbot.ini"
    if (sys.argv[1:]):
        configFilename = sys.argv[1]
        
    try:
        if not os.path.exists(configFilename):
            raise Exception()
        load_config(configFilename)
    except Exception, e:
        print >> sys.stderr, "Error while loading ini file %s" %(
            configFilename)
        print >> sys.stderr, e
        print >> sys.stderr, __doc__
        sys.exit(1)

    bot = TwitterBot(configFilename)
    return bot.run()

if __name__ == "__main__":
	main()

