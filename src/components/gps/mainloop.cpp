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
#include <orcagpsutil/latlon2mga.h>

#include "mainloop.h"

using namespace std;
using namespace orca;

namespace gps {

namespace {
    const char *SUBSYSTEM = "mainloop";
}

namespace {

    orca::GpsMapGridData convertToMapGrid( const orca::GpsData           &gpsData,
                                           const orca::Frame3d           &antennaOffset )
    {
        orca::GpsMapGridData gpsMapGridData;

        orcagpsutil::LatLon2MGA( gpsData.latitude, 
                                 gpsData.longitude,
                                 gpsMapGridData.northing,
                                 gpsMapGridData.easting,
                                 gpsMapGridData.zone);

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
MainLoop::reportBogusValues( GpsData &gpsData, GpsMapGridData &gpsMapGridData, GpsTimeData &gpsTimeData)
{
    context_.tracer()->debug("Reporting bogus values with positionType 0", 3);
    orcaice::setSane(gpsData);
    orcaice::setSane(gpsMapGridData);
    gpsData.positionType = orca::GpsPositionTypeNotAvailable;
    gpsMapGridData.positionType = orca::GpsPositionTypeNotAvailable;
    gpsInterface_->localSetAndSend(gpsData);
    gpsMapGridInterface_->localSetAndSend(gpsMapGridData);
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

        while ( isActive() )
        {
            context_.tracer()->debug("Trying to read from driver now.", 3);
            
            while ( isActive() )
            {
                // Read from hardware: blocking call with timeout, drives the loop
                int ret = hwDriver_->read();
                if (ret!=-1) break;
                string err = "Problem reading from GPS. Trying to reinitialize.";
                context_.tracer()->error( err );
                context_.status()->fault( SUBSYSTEM, err );
                hwDriver_->init();
            }
            context_.tracer()->debug("Read successfully from driver.", 3);

            if ( !hwDriver_->hasFix() )
            {
                context_.tracer()->debug("No GPS fix", 3);
                if (reportIfNoFix) reportBogusValues(gpsData, gpsMapGridData, gpsTimeData);
            }
            else
            {
                context_.tracer()->debug("We have a GPS fix", 3);
                    
                if (hwDriver_->getData(gpsData)==0) {
                    // Publish gpsData
                    context_.tracer()->debug("Publishing GpsData.", 3);
                    context_.tracer()->debug( orcaice::toString( gpsData ), 5 );
                    gpsInterface_->localSetAndSend(gpsData);

                    // Convert to MapGrid and publish
                    gpsMapGridData = convertToMapGrid( gpsData, antennaOffset_ );
                    context_.tracer()->debug("Publishing GpsMapGridData.", 3);
                    context_.tracer()->debug( orcaice::toString( gpsMapGridData ), 5 );
                    gpsMapGridInterface_->localSetAndSend(gpsMapGridData);
                } else {
                    context_.tracer()->debug("No new GpsData available. Will not publish anything", 3);    
                }

                if(hwDriver_->getTimeData(gpsTimeData)==0) {
                    context_.tracer()->debug("Publishing GpsTimeData.", 3);
                    context_.tracer()->debug( orcaice::toString( gpsTimeData ), 5 );
                    gpsTimeInterface_->localSetAndSend(gpsTimeData);
                } else {
                    context_.tracer()->debug("No new GpsTimeData available. Will not publish anything", 3);    
                }
                    
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
