/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
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
}

MainThread::~MainThread()
{
    if ( driver_ ) delete driver_;
}

void 
MainThread::initialise()
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
    
    // create a callback object to recieve data
    gpsConsumer_ = new orcaifaceimpl::StoringGpsConsumerImpl( context_ );

    activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

    subscribeToGps();
    initNetworkInterface();
    initDriver();
}

void 
MainThread::work()
{
    subStatus().setMaxHeartbeatInterval( 3.0 );

    orca::GpsData        gpsData;
    orca::Localise2dData localiseData;

    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 1000;
    // reconnect if we timeout this many times
    const int reconnectFailTimes = 10;
    int numTimeouts = 0;

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

///////////////////////////

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
        context_.tracer().debug( "driver loaded OK.",3);
    }
    else if ( driverName == "odometrybased" )
    {
        getGpsDescription();

        context_.tracer().debug( "loading 'odometrybased' driver",3);
        driver_ = new OdometryBasedDriver( gpsDescr_, vehicleDescr_, context_ );
        context_.tracer().debug( "driver loaded OK.",3);
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
            orcaice::catchExceptionsWithStatusAndSleep( "getting Gps description", subStatus(), gbxutilacfr::SubsystemFault, 2000 );
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
                context_.tracer().debug( ss.str() );

                if ( vehicleDescr_.geometry == 0 )
                {
                    subStatus().warning( "Got NULL vehicle geometry -- making something up!" );
                    orca::VehicleGeometryCylindricalDescriptionPtr geom = new orca::VehicleGeometryCylindricalDescription;
                    geom->type = orca::VehicleGeometryCylindrical;
                    geom->radius = 0.7;
                    geom->height = 1.0;
                    geom->platformToGeometryTransform = orcaobj::zeroFrame3d();
                    vehicleDescr_.geometry = geom;
                    sleep(1);
                }
                if ( vehicleDescr_.control == 0 )
                {
                    subStatus().warning( "Got NULL vehicle control -- making something up!" );
                    orca::VehicleControlVelocityDifferentialDescriptionPtr ctrl = new orca::VehicleControlVelocityDifferentialDescription;
                    ctrl->type = orca::VehicleControlVelocityDifferential;
                    ctrl->maxForwardSpeed = 1.0;
                    ctrl->maxReverseSpeed = 1.0;
                    ctrl->maxTurnrate = 90*M_PI/180.0;
                    ctrl->maxLateralAcceleration = 1.0;
                    ctrl->maxForwardAcceleration = 1.0;
                    ctrl->maxReverseAcceleration = 1.0;
                    ctrl->maxRotationalAcceleration = 90.0*M_PI/180.0;
                    vehicleDescr_.control = ctrl;
                    sleep(1);
                }

                localiseInterface_ = new orcaifaceimpl::Localise2dImpl( vehicleDescr_.geometry, "Localise2d", context_ );
                break;
            }
            catch ( ... ) {
                orcaice::catchExceptionsWithStatusAndSleep( "getting vehicle description", subStatus(), gbxutilacfr::SubsystemFault, 2000 );
            }
        }
    }
    
    //
    // Initialize Provided Interface
    //
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
