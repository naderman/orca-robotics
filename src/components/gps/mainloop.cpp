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

#include "mainloop.h"

using namespace std;
using namespace orca;

namespace gps {

namespace {
    const char *SUBSYSTEM = "mainloop";
}

namespace {

    orca::GpsMapGridData convertToMapGrid( const orca::GpsData           &gpsData,
                                           orcagpsutil::mgaMapgrid       &mgaMapgrid_,
                                           const orca::Frame3d           &antennaOffset )
    {
        orca::GpsMapGridData gpsMapGridData;

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
        p=orcaice::convertToFrame3d(antennaOffset,p);
        // reset object
        gpsMapGridData.easting=p.x;
        gpsMapGridData.northing=p.y;
        gpsMapGridData.altitude=p.z;

        return gpsMapGridData;
    }

}

MainLoop::MainLoop( GpsIfacePtr         &gpsInterface,
                    GpsMapGridIfacePtr  &gpsMapGridInterface,
                    GpsTimeIfacePtr     &gpsTimeInterface,
                    Driver              *hwDriver,
                    const orca::Frame3d &antennaOffset,
                    orcaice::Context     context )
    : gpsInterface_(gpsInterface),
      gpsMapGridInterface_(gpsMapGridInterface),
      gpsTimeInterface_(gpsTimeInterface),
      hwDriver_(hwDriver),
      antennaOffset_(antennaOffset),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
}

MainLoop::~MainLoop()
{
}

void
MainLoop::run()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string str = context_.tag() + ".Config.ReportIfNoFix";

    bool reportIfNoFix = orcaice::getPropertyAsIntWithDefault( prop, str, true );

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 5.0 );    

    try 
    {
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
                    string err = "Problem reading from GPS.  Shutting down hardware.";
                    context_.tracer()->error( err );
                    context_.status()->fault( SUBSYSTEM, err );
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
                        gpsInterface_->localSetAndSend(gpsData);
                        gpsMapGridInterface_->localSetAndSend(gpsMapGridData);
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
                        gpsInterface_->localSetAndSend(gpsData);

                        // Convert to MapGrid
                        gpsMapGridData = convertToMapGrid( gpsData, mgaMapgrid_, antennaOffset_ );

                        context_.tracer()->debug("Publishing gpsMapGridData.", 3);
                        gpsMapGridInterface_->localSetAndSend(gpsMapGridData);
                        
                        context_.tracer()->debug( orcaice::toString( gpsData ), 5 );
                        context_.tracer()->debug( orcaice::toString( gpsMapGridData ), 5 );
                    
                    }
    
                    if(hwDriver_->getTimeData(gpsTimeData)==0)
                    {
                        context_.tracer()->debug("We have new timeData. Publishing gpsTimeData.", 3);
                        gpsTimeInterface_->localSetAndSend(gpsTimeData);
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
            context_.status()->ok( SUBSYSTEM );

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

}
