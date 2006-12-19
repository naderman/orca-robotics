/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "handler.h"
#include "novatelspan/novatelspandriver.h"

using namespace std;
using namespace orca;

namespace insgps{

Handler::Handler(InsGpsI&           insGpsI,
                 insgps::Driver*    hwDriver,
                 const orcaice::Context & context )
    : insGpsI_(&insGpsI),
      hwDriver_(hwDriver),
      context_(context)
{
}
   
void
Handler::run()
{
    const int TIME_BETWEEN_HEARTBEATS  = 10000;  // ms
    IceUtil::Time lastHeartbeatTime = IceUtil::Time::now();

    try 
    {
        context_.tracer()->debug( "TRACE(handler::run()): Handler thread is running", 6);
        
        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks
        // for requests to shut down. So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
        {

            try
            {
                               
                if ( hwDriver_->isEnabled() )
                {
                    // blocking read with timeout (2000ms by default)
                    // get & send the gps data to icestorm and to a buffer for direct connections
                    
                    // context_.tracer()->debug( "TRACE(handler::run()): publishing data", 5 );
                    insGpsI_->publish();
                    // context_.tracer()->debug( "TRACE(handler::run()): published data", 5 );
                }
                else
                {
                    // Wait for someone to enable us
                    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
                }

                if ( (IceUtil::Time::now()-lastHeartbeatTime).toMilliSecondsDouble() >= TIME_BETWEEN_HEARTBEATS )
                {
                    if ( hwDriver_->isEnabled() )
                    {
                        context_.status()->heartbeat("InsGps enabled. " + hwDriver_->heartbeatMessage() );
                    }
                    else
                    {
                        context_.status()->heartbeat( "InsGps disabled." );
                    }
                    lastHeartbeatTime = IceUtil::Time::now();
                } 
            } // end of try
            catch ( insgps::NovatelSpanException& e )
            {
                // this ok, data is not available yet
            }
        } // end of while
    } // end of try
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    // InsGps hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 6 );
            
}

}
