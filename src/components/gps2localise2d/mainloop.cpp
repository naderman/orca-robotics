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
#include <orca/odometry2d.h>
#include "simpledriver.h"
#include "odometrybaseddriver.h"
#include "mainloop.h"

using namespace std;

namespace gps2localise2d {

namespace {
    const char *SUBSYSTEM = "mainloop";
}

MainLoop::MainLoop( const orcaice::Context &context )
    : driver_(NULL),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
    
    // create a callback object to recieve data
    gpsConsumer_ = new orcaifaceimpl::ProxiedGpsConsumerImpl( context_ );
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
    else if ( driverName == "odometrybased" )
    {
        connectToGps();
        getGpsDescription();

        context_.tracer()->debug( "loading 'odometrybased' driver",3);
        driver_ = new OdometryBasedDriver( gpsDescr_, context_ );
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

    context_.tracer()->info( "Subscribed to gps." );
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
    orca::VehicleDescription vehicleDesc;
    
    std::string prefix = context_.tag() + ".Config.";
    bool requireOdometry = orcaice::getPropertyAsIntWithDefault( context_.properties(),
    prefix+"RequireOdometry", 1);
    
    if (!requireOdometry)
    {
        context_.tracer()->debug("Odometry interface is not required: VehicleDescription is set to unknown", 3);
        orca::VehicleGeometryDescriptionPtr geometry = new orca::VehicleGeometryDescription;
        geometry->type = orca::VehicleGeometryOther;
        vehicleDesc.geometry = geometry; 
    }
    else
    {
        //
        // connect to odometry to get vehicle description
        //
        
        orca::Odometry2dPrx odoPrx;
        
        while ( isActive() )
        {
            try
            {
                context_.tracer()->debug( "Connecting to Odometry2d...", 3 );
                orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odoPrx, "Odometry2d" );
                context_.tracer()->debug("connected to a 'Odometry2d' interface", 4 );
                context_.tracer()->debug( "Getting vehicle description...", 2 );
                vehicleDesc = odoPrx->getDescription();
                stringstream ss;
                ss << "Got vehicle description: " << orcaice::toString( vehicleDesc );
                context_.tracer()->info( ss.str() );
                break;
            }
            catch ( const Ice::Exception &e )
            {
                stringstream ss;
                ss << "failed to retrieve vehicle description: " << e;
                context_.tracer()->error( ss.str() );
            }
            catch ( const std::exception &e )
            {
                stringstream ss;
                ss << "failed to retreive vehicle description: " << e.what();
                context_.tracer()->error( ss.str() );
            }
            catch ( ... )
            {
                context_.tracer()->error( "Failed to retreive vehicle description for unknown reason." );
            }
            context_.status()->initialising( SUBSYSTEM, "getVehicleDescription()" );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }
    
    //
    // Instantiate External Interface
    //
    localiseInterface_ = new orcaifaceimpl::Localise2dImpl( vehicleDesc.geometry, "Localise2d", context_ );
    
    while ( isActive() )
    {
        try {
            context_.tracer()->debug( "Initialising Localise2d interface...",3 );
            localiseInterface_->initInterface();
            context_.tracer()->debug( "Initialised Localise2d interface",3 );
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
    const int timeoutMs = 1000;
    // reconnect if we timeout this many times
    const int reconnectFailTimes = 10;
    int numTimeouts = 0;

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
                if ( numTimeouts++ > reconnectFailTimes )
                {
                    stringstream ss;
                    ss << "Timed out (" << timeoutMs << "ms) waiting for data.  Reconnecting.";
                    context_.tracer()->warning( ss.str() );
                    context_.status()->warning( SUBSYSTEM, ss.str() );
                    connectToGps();
                }
                continue;
            }
            numTimeouts = 0;

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

            localiseInterface_->localSetAndSend( localiseData );

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
