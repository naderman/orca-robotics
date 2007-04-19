/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "mainloop.h"
#include "simpledriver.h"

using namespace std;

namespace gps2localise2d {

namespace {
    const char *SUBSYSTEM = "mainloop";
}

MainLoop::MainLoop( orcaifaceimpl::Localise2dIface &localiseInterface,
                    const orcaice::Context &context )
    : driver_(NULL),
      localiseInterface_(localiseInterface),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );

//     sensorOffset_.p.x=0;
//     sensorOffset_.p.y=0;
//     sensorOffset_.p.z=0;
//     sensorOffset_.o.r=0;
//     sensorOffset_.o.p=0;
//     sensorOffset_.o.y=0;

    // create a callback object to recieve data
    gpsConsumer_ = new orcaifaceimpl::proxiedGpsConsumer( context_ );
}

MainLoop::~MainLoop()
{
    if ( driver_ ) delete driver_;
}

void 
MainLoop::initDriver()
{
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();
    std::string driverName = orcaice::getPropertyWithDefault( prop,
            prefix+"Driver", "simple" );
    
    if ( driverName == "simple" )
    {
        connectToGps();
        getGpsDescription();

        context_.tracer()->debug( "loading 'simple' driver",3);
        driver_ = new SimpleDriver( gpsDescr_, context_ );
    }
    else
    {
        std::string errString = "Unknown driver: " + driverName;
        context_.tracer()->error( errString );
        exit(1);
    }
    
    context_.tracer()->debug("driver instantiated",5);
}

void 
MainLoop::connectToGps()
{
    while ( isActive() )
    {
        try
        {
            context_.tracer()->debug( "Connecting to gps...", 3 );
            orcaice::connectToInterfaceWithTag<orca::GpsPrx>( context_, gpsPrx_, "Gps" );
            context_.tracer()->debug("connected to a 'Gps' interface", 4 );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to connect to gps interface: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to connect to gps interface: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Failed to connect to gps interface for unknown reason." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    while ( isActive() )
    {
        try
        {
            context_.tracer()->debug( "Subscribing to gps...", 3 );
            gpsPrx_->subscribe( gpsConsumer_->consumerPrx() );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to subscribe to gps: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to subscribe to gps: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Failed to subscribe to gps for unknown reason." );
        }
        context_.status()->initialising( SUBSYSTEM, "connectToGps()" );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    context_.tracer()->info( "Connected to gps." );
}

void 
MainLoop::getGpsDescription()
{
    while ( isActive() )
    {
        try
        {
            context_.tracer()->debug( "Getting gps description...", 2 );
            gpsDescr_ = gpsPrx_->getDescription();
            stringstream ss;
            ss << "Got gps description: " << orcaice::toString( gpsDescr_ );
            context_.tracer()->info( ss.str() );
            antennaOffset_ = gpsDescr_.antennaOffset;
            if ( antennaOffsetOK( antennaOffset_ ) )
                return;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to retreive gps description: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to retreive gps description: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Failed to retreive gps description for unknown reason." );
        }
        context_.status()->initialising( SUBSYSTEM, "getGpsDescription()" );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
MainLoop::initInterface()
{
    while ( isActive() )
    {
        try {
            context_.tracer()->debug( "Initialising PolarFeature2d interface...",3 );
            localiseInterface_.initInterface();
            context_.tracer()->debug( "Initialised PolarFeature2d interface",3 );
            return;
        }
        catch ( orcaice::Exception &e )
        {
            context_.tracer()->warning( std::string("MainLoop::initInterface(): ") + e.what() );
        }
        context_.status()->initialising( SUBSYSTEM, "initInterface()" );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void 
MainLoop::run()
{
    initInterface();
    initDriver();

    orca::GpsData        gpsData;
    orca::Localise2dData localiseData;

    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 1500;

    context_.tracer()->debug( "Entering main loop.",2 );
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 3.0 );

    // Loop forever till we get shut down.
    while ( isActive() )
    {
        try
        {                
            //
            // block on arrival of data
            //
            int ret = gpsConsumer_->proxy().getNext ( gpsData, timeoutMs );
            if ( ret != 0 ) {
                stringstream ss;
                ss << "Timed out (" << timeoutMs << "ms) waiting for data.  Reconnecting.";
                context_.tracer()->warning( ss.str() );
                context_.status()->warning( SUBSYSTEM, ss.str() );
                connectToGps();
                continue;
            }

            //
            // execute algorithm to compute localisation
            //
            bool canCompute = driver_->compute( gpsData, localiseData );

            if ( !canCompute )
            {
                context_.status()->ok( SUBSYSTEM );
                context_.tracer()->debug( "MainLoop: can't compute localiseData" );
                continue;
            }

            // copy the timestamp
            localiseData.timeStamp = gpsData.timeStamp;

            localiseInterface_.localSetAndSend( localiseData );

            context_.status()->ok( SUBSYSTEM );

        } // try
        catch ( const orca::OrcaException & e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected std::string exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "MainLoop: unexpected exception from somewhere.");
        }
    } // while

    context_.tracer()->debug( "Exited main loop.",2 );
}

bool
MainLoop::antennaOffsetOK( const orca::Frame3d &offset )
{
    bool offsetOk = true;
    if ( offset.o.r != 0.0 || offset.o.p != 0.0 || offset.o.y != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero roll/pitch/yaw in antenna offset. Offset: " << orcaice::toString(offset);
        context_.tracer()->error( ss.str() );
        offsetOk = false;
    }

    return offsetOk;
}

}
