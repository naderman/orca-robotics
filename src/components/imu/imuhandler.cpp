/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006 Ben Upcroft
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "imuhandler.h"

using namespace std;
using namespace orca;
using namespace orcaice;

ImuHandler::ImuHandler(ImuI&             imuObj,
                       Position3dI&      position3dObj,              
                       ImuDriver*        hwDriver,
                       orcaice::Context  context,
                       bool              startEnabled )
    : imuObj_(imuObj),
      position3dObj_(position3dObj),
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
	ImuConfigDataPtr config_ = imuObj_.localGetConfig();
        ImuDataPtr imuData = new ImuData;
        Position3dDataPtr position3dData = new Position3dData;
        
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
                if( hwDriver_->getData( position3dData ) == 0 )
                {
                    // raw imu data
                    position3dObj_.localSetData( position3dData );
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
                    context_.tracer()->heartbeat("Imu enabled. " + hwDriver_->heartbeatMessage() );
                }
                else
                {
                    context_.tracer()->heartbeat( "Imu disabled." );
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
