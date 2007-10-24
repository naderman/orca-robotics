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

MainLoop::MainLoop( orcaifaceimpl::GpsImplPtr &gpsInterface,
                    Driver                    *hwDriver,
                    const orca::Frame3d       &antennaOffset,
                    orcaice::Context           context )
    : gpsInterface_(gpsInterface),
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
MainLoop::reportBogusValues( GpsData &gpsData )
{
    context_.tracer()->debug("Reporting bogus values with positionType 0", 3);
    orcaice::setSane(gpsData);
    gpsData.positionType = orca::GpsPositionTypeNotAvailable;
    gpsInterface_->localSetAndSend(gpsData);
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

        while ( !isStopping() )
        {
            context_.tracer()->debug("Trying to read from driver now.", 3);
            
            while ( !isStopping() )
            {
                try 
                {
                    // Read from hardware: blocking call with timeout, drives the loop
                    hwDriver_->read(gpsData);
                    break;
                }
                catch (GpsException &e)
                {
                    stringstream ss;
                    ss << "MainLoop: Problem reading from GPS: " << e.what();
                    context_.tracer()->error( ss.str() );
                    context_.status()->fault( SUBSYSTEM, ss.str() );
                }

                //If the read threw then we should now try to re-initialise 
                try 
                {
                    context_.tracer()->debug("Trying to reinitialize now", 2);
                    hwDriver_->init();
                }
                catch (GpsException &e)
                {
                    stringstream ss;
                    ss << "MainLoop: Problem reinitializing: " << e.what();
                    context_.tracer()->error( ss.str() );
                }
                    
            }
            context_.tracer()->debug("Read successfully from driver.", 3);



            if ( gpsData.positionType == orca::GpsPositionTypeNotAvailable ){
                context_.tracer()->debug("No GPS fix", 2);
                if (reportIfNoFix) reportBogusValues(gpsData);
            }else{
                context_.tracer()->debug("We have a GPS fix", 2);
                    
                // Publish gpsData
                context_.tracer()->debug("New GpsData: publishing now.", 3);
                context_.tracer()->debug( orcaice::toString( gpsData ), 5 );
                gpsInterface_->localSetAndSend(gpsData);
            }        
                
            context_.status()->ok( SUBSYSTEM );

        } // end of while
    } // end of try

    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "MainLoop:: Caught exception: " << e;
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
    }
    catch ( const std::exception & e )
    {
        stringstream ss;
        ss << "MainLoop: Caught exception: " << e.what();
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
    }
    catch ( ... )
    {
        context_.tracer()->error( "MainLoop: caught unknown unexpected exception.");
        context_.status()->fault( SUBSYSTEM, "MainLoop: caught unknown unexpected exception.");
    }

    // GPS hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 5 );
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}

}
