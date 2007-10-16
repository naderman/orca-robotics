#
#  Orca Project: Components for robotics 
#               http://orca-robotics.sf.net/
#  Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
# 
#  This copy of Orca is licensed to you under the terms described in the
#  ORCA_LICENSE file included in this distribution.
#
#

# Example of using the Ice python API directly to perform a remote call to a laser and print
# the laser data once. 

import sys, traceback, Ice
from socket import gethostname

# from orca import laserscanner2d
import orca

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

    # Create a proxy for the laser
    #
    # we'll use indirect proxy here to connect to the laser2d component
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
    
    print "Derived proxy created..."

    # get one laser scan and print it out
    #
    data = laser.getData()

    print "Received laser scan:"
    print data

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