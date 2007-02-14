/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "gpshandler.h"

using namespace std;
using namespace orca;
using namespace orcaice;

GpsHandler::GpsHandler(GpsI              &gpsObj,
                       GpsDriver         *hwDriver,
                       orcaice::Context  context,
                       bool              startEnabled )
    : gpsObj_(gpsObj),
      hwDriver_(hwDriver),
      context_(context)
{
}

GpsHandler::~GpsHandler()
{
}

void
GpsHandler::run()
{
    const int TIME_BETWEEN_HEARTBEATS  = 10000;  // ms
    IceUtil::Time lastHeartbeatTime = IceUtil::Time::now();

    try 
    {
        GpsDescription descr_ = gpsObj_.localGetDescription();
        GpsData gpsData;
        GpsMapGridData gpsMapGridData;
        GpsTimeData gpsTimeData;

        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
        //            So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
        {
            //
            // This 'if' block is what slows the loop down, by either reading from the GPS
            // or sleeping.
            //
            if ( hwDriver_->isEnabled() )
            {
                // Read from the GPS
                context_.tracer()->debug("Trying to read from driver now", 3);
                
                // GPS data tends to come in bursts in 1 second intervals
                // hwDriver_->read() will return after emptying serial buffer
                // this should provide a nice 1s loop frequency
                // may need to be adjusted for fancier GPSes                
                int ret = hwDriver_->read();
                if ( ret == -1 )
                {
                    context_.tracer()->error( "Problem reading from GPS.  Shutting down hardware." );
                    hwDriver_->disable();
                }

                if ( !hwDriver_->hasFix() )
                {
                    context_.tracer()->debug("No GPS fix", 3);
                }
                else
                {
                    context_.tracer()->debug("We have a GPS fix", 3);
                        
                    if(hwDriver_->getData(gpsData)==0)
                    {
                        // publish it
                        context_.tracer()->debug("We have new gpsData. Publishing gpsData.", 3);
                        context_.tracer()->debug("Publishing gpsData.", 3);
                        gpsObj_.localSetData(gpsData);
                        
                        int zone;
                        zone=mgaMapgrid_.getGridCoords( gpsData.latitude, gpsData.longitude,
                                                        gpsMapGridData.easting,gpsMapGridData.northing);
                        gpsMapGridData.zone=zone;
                        //copy across all the other stuff
                        gpsMapGridData.timeStamp=gpsData.timeStamp;
                        gpsMapGridData.utcTime=gpsData.utcTime;
                        gpsMapGridData.altitude=gpsData.altitude;
                    
                        gpsMapGridData.heading=gpsData.heading;
                        gpsMapGridData.speed=gpsData.speed;
                        gpsMapGridData.climbRate=gpsData.climbRate;
                        gpsMapGridData.positionType=gpsData.positionType;
                    
                        //correct for local frame
                        CartesianPoint p;
                        //copy out point
                        p.x=gpsMapGridData.easting;
                        p.y=gpsMapGridData.northing;
                        p.z=gpsMapGridData.altitude;
                        //convert
                        p=convertToFrame3d(descr_.antennaOffset,p);
                        // reset object
                        gpsMapGridData.easting=p.x;
                        gpsMapGridData.northing=p.y;
                        gpsMapGridData.altitude=p.z;
                    
                        context_.tracer()->debug("Publishing gpsMapGridData.", 3);
                        gpsObj_.localSetMapGridData(gpsMapGridData);
                        
                        context_.tracer()->debug( orcaice::toString( gpsData ), 5 );
                        context_.tracer()->debug( orcaice::toString( gpsMapGridData ), 5 );
                    
                    }
    
                    if(hwDriver_->getTimeData(gpsTimeData)==0)
                    {
                        context_.tracer()->debug("We have new timeData. Publishing gpsTimeData.", 3);
                        gpsObj_.localSetTimeData(gpsTimeData);
                        context_.tracer()->debug( orcaice::toString( gpsTimeData ), 5 );
                    }
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
                    context_.status()->heartbeat("Gps enabled. " + hwDriver_->heartbeatMessage() );
                }
                else
                {
                    context_.status()->heartbeat( "Gps disabled." );
                }
                lastHeartbeatTime = IceUtil::Time::now();
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
    
    // GPS hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 5 );
}
