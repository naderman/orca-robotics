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
    
    Ice::PropertiesPtr prop = context_.properties();
    std::string str = context_.tag() + ".Config.ReportIfNoFix";

    bool reportIfNoFix = orcaice::getPropertyAsIntWithDefault( prop, str, true );

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
            
            // Loop is driven by waiting time of 1/2 second at the bottom
            
            if ( hwDriver_->isEnabled() )
            {
                context_.tracer()->debug("Trying to read from driver now", 3);
                
                // Read from hardware: parse all information in the serial buffer until empty
                // as quickly as possible
                int ret = hwDriver_->read();
                if ( ret == -1 )
                {
                    context_.tracer()->error( "Problem reading from GPS.  Shutting down hardware." );
                    hwDriver_->disable();
                }

                if ( !hwDriver_->hasFix() )
                {
                    context_.tracer()->debug("No GPS fix", 3);
                    if (reportIfNoFix) {
                        context_.tracer()->debug("Reporting bogus values with positionType 0", 3);
                        orcaice::setSane(gpsData);
                        orcaice::setSane(gpsMapGridData);
                        gpsData.positionType = orca::GpsPositionTypeNotAvailable;
                        gpsMapGridData.positionType = orca::GpsPositionTypeNotAvailable;
                        gpsObj_.localSetData(gpsData);
                        gpsObj_.localSetMapGridData(gpsMapGridData);
                    }
                }
                else
                {
                    context_.tracer()->debug("We have a GPS fix", 3);
                        
                    int ret = hwDriver_->getData(gpsData);
                    if (ret==0)
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
                        gpsMapGridData.horizontalPositionError=gpsData.horizontalPositionError;
                        gpsMapGridData.verticalPositionError=gpsData.verticalPositionError;
                    
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
                
                // Tobi: this sleep drives the loop now
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(500));
                
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
