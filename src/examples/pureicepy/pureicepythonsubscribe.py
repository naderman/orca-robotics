#
#  Orca Project: Components for robotics 
#               http://orca-robotics.sf.net/
#  Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
# 
#  This copy of Orca is licensed to you under the terms described in the
#  ORCA_LICENSE file included in this distribution.
#
#

# Example of using the Ice python API directly to subscribe to a laser and print the
# incoming data. 

import sys, traceback, Ice
from socket import gethostname
from pylab import *
import orca

#
# Implementation of the servant for subscribing 
# Based off the RangeScanner2dConsumerI in the lasermon component
#
class RangeScanner2dConsumerI(orca.RangeScanner2dConsumer):
    def setData(self, data, current=None):
	try :
	    print data
	except:
	    raise "Something went wrong when setData was called"


#
# This client code is based on the example from
# Chapter 23 Developing a File System Client in Python
# in the Ice User Manual.
#
status = 0
ic = None
base = None
laser = None
try: 
    # Create a communicator
    #
    ic = Ice.initialize(sys.argv)
    print "Communicator initialized..."

    # Create a proxy for the laser so we can do remote calls on the laser server
    #
    # we'll use indirect proxy here to connect to the sicklaser component
    # setup according to the Quick Start tutorial of Orca2
    # see: orca-robotics.sf.net/orca_doc_quickstart.html
    base = ic.stringToProxy( "laserscanner2d@"+gethostname()+"/laser2d" )
    
    # check that the object exists otherwise this will raise an exceptions
    base.ice_ping();
    
    print "Base proxy created..."

    # Down-cast the proxy to a Laser proxy
    #
    laser = orca.LaserScanner2dPrx.checkedCast(base)
    if laser == None:
        raise "Invalid proxy - could not downcast to a laserscanner2d proxy"

    # get one laser scan using a remote call via the proxy and print it out
    #
    data = laser.getData()

    print "Received laser scan:"
    print data

    #
    # Servant for subscribing to the laser server
    #
    
    # first create the adaptor
    adapter = ic.createObjectAdapterWithEndpoints( "LaserConsumerAdapter", "default -p 2000")
    # create a servant by instantiating a consumer object
    consumer = RangeScanner2dConsumerI()
    # inform the object adapter of the presence of the new consumer servant
    # and return the proxy to that servant so that we can pass it to the laser
    # server when we call subscribe
    consumerObj = adapter.add( consumer, Ice.stringToIdentity("LaserConsumer") )
    # make a direct proxy
    prx = adapter.createDirectProxy( consumerObj.ice_getIdentity() );
    # downcast the proxy to a rangescanner proxy
    consumerPrx = orca.RangeScanner2dConsumerPrx.uncheckedCast( prx );
    if consumerPrx == None:
	raise "Invalid proxy - could not downcast to a RangeScanner2dConsumer proxy"

    adapter.activate()


    #
    # subscribe to the laser server
    #
    
    # pass the consumer proxy to the laser server
    laser.subscribe( consumerPrx )

    ic.waitForShutdown()
    
except: 
    traceback.print_exc()
    status = 1

if ic:
    # Clean up
    #
    try:
        ic.destroy()
    except:
        traceback.print_exc()
        status = 1

sys.exit(status) 