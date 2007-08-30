#
#  Orca Project: Components for robotics 
#               http://orca-robotics.sf.net/
#  Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
# 
#  This copy of Orca is licensed to you under the terms described in the
#  ORCA_LICENSE file included in this distribution.
#
#

# Example of using the Ice python API directly to subscribe to a laser and plot the
# incoming data in 3D. Here the 3d data is just made up but is redrawn every time a
# laser scan is received

import sys, traceback, Ice
from socket import gethostname
from numpy import *
import pylab as p
import matplotlib.axes3d as p3
import orca

#
# Implementation of the servant for subscribing 
#
class RangeScanner2dConsumerI( orca.RangeScanner2dConsumer ):
    def __init__(self):
	# private data attributes
	self.__i = 0
	self.__initialise = 1
    
    # remote call for servers to shove data into
    def setData(self, data, current=None):
	try :
	    if self.__initialise:
		# u and v are parametric variables.
		# u is an array from 0 to 2*pi, with 100 elements
		u=r_[0:2*pi:100j]
		# v is an array from 0 to 2*pi, with 100 elements
		v=r_[0:pi:100j]
		
		# x, y, and z are the coordinates of the points for plotting
		# each is arranged in a 100x100 array
		self.__x=10*outer(cos(u),sin(v))
		self.__y=10*outer(sin(u),sin(v))
		self.__z=10*outer(ones(size(u)),cos(v))
		self.__fig=p.figure()
		self.__ax = p3.Axes3D(self.__fig)
    
		self.__ax.plot_wireframe(self.__x, self.__y, self.__z)
		self.__initialise = 0
	    
	    if self.__i:
		self.__ax.set_xlabel('alternating labels to show incoming data')
		self.__i = 0
	    else:
		self.__ax.set_xlabel('')
		self.__i = 1
	    
	    # redraw the plot
	    p.draw()
	    
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


    # set up figures for drawing
    p.ion()

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