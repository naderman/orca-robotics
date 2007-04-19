/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "imuhandler.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace imu {

ImuHandler::ImuHandler(ImuI                            &imuObj,
                       orcaifaceimpl::Odometry3dIface  &odometry3dObj,
                       ImuDriver                       *hwDriver,
                       orcaice::Context                context,
                       bool                            startEnabled )
    : imuObj_(imuObj),
      odometry3dObj_(odometry3dObj),
      hwDriver_(hwDriver),
      context_(context)
{
}

ImuHandler::~ImuHandler()
{
}

void
ImuHandler::run()
{
    const int TIME_BETWEEN_HEARTBEATS  = 10000;  // ms
    IceUtil::Time lastHeartbeatTime = IceUtil::Time::now();

    try 
    {
//         orca::ImuDescription imuDescr = imuObj_.localGetDescription();
//         orca::Position3dDescription pos3dDescr = position3dObj_.localGetDescription(); 
        ImuData imuData;
        Odometry3dData odometry3dData;

        
        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
        //            So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
        {
            //
            // This 'if' block is what slows the loop down, by either reading from the IMU
            // or sleeping.
            //
            if ( hwDriver_->isEnabled() )
	    {

                // Read from the IMU
                int ret = hwDriver_->read();
                if ( ret == -1 )
                {
                    context_.tracer()->error( "Problem reading from IMU.  Shutting down hardware." );
                    hwDriver_->disable();
		}

                // get the raw imu measurements
                if( hwDriver_->getData( imuData ) == 0 )
                {
                    // raw imu data
                    imuObj_.localSetData( imuData );
		}
                
                // get the pva data              
                if( hwDriver_->getData( odometry3dData ) == 0 )
                {
                    // raw imu data
                    odometry3dObj_.localSetAndSend( odometry3dData );
                }
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
                    context_.status()->heartbeat("Imu enabled. " + hwDriver_->heartbeatMessage() );
                }
                else
                {
                    context_.status()->heartbeat( "Imu disabled." );
                }
                lastHeartbeatTime = IceUtil::Time::now();
            }
        } // end of while
    } // end of try
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    // IMU hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 5 );
}

} //namespace
