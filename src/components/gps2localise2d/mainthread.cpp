/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orca/odometry2d.h>
#include "simpledriver.h"
#include "odometrybaseddriver.h"
#include "mainthread.h"

using namespace std;
using namespace gps2localise2d;


MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    driver_(NULL),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
    
    // create a callback object to recieve data
    gpsConsumer_ = new orcaifaceimpl::StoringGpsConsumerImpl( context_ );
}

MainThread::~MainThread()
{
    if ( driver_ ) delete driver_;
}

void 
MainThread::initDriver()
{
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();
    std::string driverName = orcaice::getPropertyWithDefault( prop,
            prefix+"Driver", "simple" );
    
    if ( driverName == "simple" )
    {
        getGpsDescription();

        context_.tracer().debug( "loading 'simple' driver",3);
        driver_ = new SimpleDriver( gpsDescr_, vehicleDescr_, context_ );
    }
    else if ( driverName == "odometrybased" )
    {
        getGpsDescription();

        context_.tracer().debug( "loading 'odometrybased' driver",3);
        driver_ = new OdometryBasedDriver( gpsDescr_, vehicleDescr_, context_ );
    }
    else
    {
        std::string errString = "Unknown driver: " + driverName;
        context_.tracer().error( errString );
        exit(1);
    }
    
    context_.tracer().debug("driver instantiated",5);
}

void 
MainThread::subscribeToGps()
{
    gpsConsumer_->subscribeWithTag( "Gps", this, subsysName() );
    context_.tracer().info( "Connected and subscribed to gps." );
}

void 
MainThread::getGpsDescription()
{
    while ( !isStopping() )
    {
        orca::GpsPrx gpsPrx;
        try
        {    
            orcaice::connectToInterfaceWithTag<orca::GpsPrx>( context_, gpsPrx, "Gps" );

            context_.tracer().debug( "Getting gps description...", 2 );
            gpsDescr_ = gpsPrx->getDescription();
            stringstream ss;
            ss << "Got gps description: " << orcaobj::toString( gpsDescr_ );
            context_.tracer().info( ss.str() );
            antennaOffset_ = gpsDescr_.antennaOffset;
            if ( antennaOffsetOK( antennaOffset_ ) )
                return;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "getting Gps description", subStatus(), gbxutilacfr::SubsystemStatusFault, 2000 );
        }
    }
}

void
MainThread::initNetworkInterface()
{   
    std::string prefix = context_.tag() + ".Config.";
    bool requireOdometry = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"RequireOdometry", 1);
    
    if (!requireOdometry)
    {
        context_.tracer().debug("Odometry interface is not required: VehicleDescription is set to unknown", 3);
        orca::VehicleGeometryDescriptionPtr geometry = new orca::VehicleGeometryDescription;
        geometry->type = orca::VehicleGeometryOther;
        vehicleDescr_.geometry = geometry; 
    }
    else
    {
        //
        // connect to odometry to get vehicle description
        //
        
        // not using a single-line multi-try function here, because in addition to connecting to the interface
        // we need to get the vehicle description.
        orca::Odometry2dPrx odoPrx;
        
        while ( !isStopping() )
        {
            try
            {
                context_.tracer().debug( "Connecting to Odometry2d...", 3 );
                orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odoPrx, "Odometry2d" );
                context_.tracer().debug("connected to a 'Odometry2d' interface", 4 );
                context_.tracer().debug( "Getting vehicle description...", 2 );
                vehicleDescr_ = odoPrx->getDescription();
                stringstream ss;
                ss << "Got vehicle description: " << orcaobj::toString( vehicleDescr_ );
                context_.tracer().info( ss.str() );
                break;
            }
            catch ( ... ) {
                orcaice::catchExceptionsWithStatusAndSleep( "initialising network interface", subStatus(), gbxutilacfr::SubsystemStatusFault, 2000 );
            }
        }
    }
    
    //
    // Initialize Provided Interface
    //
    localiseInterface_ = new orcaifaceimpl::Localise2dImpl( vehicleDescr_.geometry, "Localise2d", context_ );
    localiseInterface_->initInterface( this, subsysName() );
}

bool
MainThread::antennaOffsetOK( const orca::Frame3d &offset )
{
    bool offsetOk = true;
    if ( offset.o.r != 0.0 || offset.o.p != 0.0 || offset.o.y != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero roll/pitch/yaw in antenna offset. Offset: " << orcaobj::toString(offset);
        context_.tracer().error( ss.str() );
        offsetOk = false;
    }

    return offsetOk;
}

void 
MainThread::walk()
{
    activate( context_, this, subsysName() );

    subscribeToGps();
    initNetworkInterface();
    initDriver();

    orca::GpsData        gpsData;
    orca::Localise2dData localiseData;

    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 1000;
    // reconnect if we timeout this many times
    const int reconnectFailTimes = 10;
    int numTimeouts = 0;

    context_.tracer().debug( "Entering main loop.",2 );
    subStatus().setMaxHeartbeatInterval( 3.0 );

    // Loop forever till we get shut down.
    while ( !isStopping() )
    {
        try
        {                
            //
            // block on arrival of data
            //
            int ret = gpsConsumer_->store().getNext ( gpsData, timeoutMs );
            subStatus().heartbeat();

            if ( ret != 0 ) {
                if ( numTimeouts++ > reconnectFailTimes )
                {
                    stringstream ss;
                    ss << "Timed out (" << timeoutMs << "ms) waiting for data.  Reconnecting.";
                    context_.tracer().warning( ss.str() );
                    subStatus().warning( ss.str() );
                    subscribeToGps();
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
                subStatus().ok();
                context_.tracer().debug( "MainThread: can't compute localiseData" );
                continue;
            }

            // copy the timestamp
            localiseData.timeStamp = gpsData.timeStamp;

            localiseInterface_->localSetAndSend( localiseData );

            subStatus().ok();

        } // try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }
    } // while
}
