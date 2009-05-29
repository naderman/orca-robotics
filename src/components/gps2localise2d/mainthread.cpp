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
#include <orca/odometry2d.h>
#include "simpledriver.h"
#include "odometrybaseddriver.h"
#include "mainthread.h"
#include <orcaifaceutil/gps.h>
#include <orcaobj/bros1.h>

using namespace std;
using namespace gps2localise2d;

namespace {

    // This component is 2D-centric: can only handle certain orientations.
    // Throw exceptions if it's otherwise.
    void
    checkAntennaOffsetOK( const orca::Frame3d &offset )
    {
        if ( offset.o.r != 0.0 || offset.o.p != 0.0 || offset.o.y != 0.0 )
        {
            stringstream ss;
            ss << "Can't handle non-zero roll/pitch/yaw in antenna offset. Offset: " << ifaceutil::toString(offset);
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    orca::VehicleDescription
    getVehicleDescr( orcaice::Context       &context,
                     gbxutilacfr::Stoppable &activity,
                     gbxutilacfr::SubHealth &subHealth )
    {   
        std::string prefix = context.tag() + ".Config.";
        bool requireOdometry = orcaice::getPropertyAsIntWithDefault( context.properties(), prefix+"RequireOdometry", 1);

        orca::VehicleDescription vehicleDescr;
    
        if (!requireOdometry)
        {
            context.tracer().debug("Odometry interface is not required: VehicleDescription is set to unknown", 3);
            orca::VehicleGeometryDescriptionPtr geometry = new orca::VehicleGeometryDescription;
            geometry->type = orca::VehicleGeometryOther;
            vehicleDescr.geometry = geometry; 
        }
        else
        {
            //
            // connect to odometry to get vehicle description
            //
        
            // not using a single-line multi-try function here, because in addition to connecting to the interface
            // we need to get the vehicle description.
            orca::Odometry2dPrx odoPrx;
        
            while ( !activity.isStopping() )
            {
                try
                {
                    context.tracer().debug( "Connecting to Odometry2d...", 3 );
                    orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context, odoPrx, "Odometry2d" );
                    context.tracer().debug("connected to a 'Odometry2d' interface", 4 );
                    context.tracer().debug( "Getting vehicle description...", 2 );
                    vehicleDescr = odoPrx->getDescription();
                    stringstream ss;
                    ss << "Got vehicle description: " << ifaceutil::toString( vehicleDescr );
                    context.tracer().debug( ss.str() );

                    if ( vehicleDescr.geometry == 0 )
                    {
                        subHealth.warning( "Got NULL vehicle geometry -- making something up!" );
                        orca::VehicleGeometryCylindricalDescriptionPtr geom = new orca::VehicleGeometryCylindricalDescription;
                        geom->type = orca::VehicleGeometryCylindrical;
                        geom->radius = 0.7;
                        geom->height = 1.0;
                        geom->platformToGeometryTransform = orcaobj::zeroFrame3d();
                        vehicleDescr.geometry = geom;
                        sleep(1);
                    }
                    if ( vehicleDescr.control == 0 )
                    {
                        subHealth.warning( "Got NULL vehicle control -- making something up!" );
                        orca::VehicleControlVelocityDifferentialDescriptionPtr ctrl = new orca::VehicleControlVelocityDifferentialDescription;
                        ctrl->type = orca::VehicleControlVelocityDifferential;
                        ctrl->maxForwardSpeed = 1.0;
                        ctrl->maxReverseSpeed = 1.0;
                        ctrl->maxTurnrate = 90*M_PI/180.0;
                        ctrl->maxLateralAcceleration = 1.0;
                        ctrl->maxForwardAcceleration = 1.0;
                        ctrl->maxReverseAcceleration = 1.0;
                        ctrl->maxRotationalAcceleration = 90.0*M_PI/180.0;
                        vehicleDescr.control = ctrl;
                        sleep(1);
                    }

                    break;
                }
                catch ( ... ) {
                    orcaice::catchExceptionsWithStatusAndSleep( "getting vehicle description", subHealth, gbxutilacfr::SubsystemFault, 2000 );
                }
            }
        }
        return vehicleDescr;
    }

}

//////////////////////////////////////////////////////////////////////

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
    setMaxHeartbeatInterval( 10.0 );

    // create a callback object to recieve data
    gpsConsumer_ = new orcaifaceimpl::StoringGpsConsumerImpl( context_ );

    subscribeToGps();
    orca::VehicleDescription vehicleDescr = getVehicleDescr( context_, *this, health() );
    initDriver( vehicleDescr );
    
    //
    // Initialize Provided Interface
    //
    localiseInterface_ = new orcaifaceimpl::Localise2dImpl( vehicleDescr.geometry, "Localise2d", context_ );
    localiseInterface_->initInterface( this, subsysName() );    
}

void 
MainThread::work()
{
    setMaxHeartbeatInterval( 3.0 );

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
            health().heartbeat();

            if ( ret != 0 ) {
                if ( numTimeouts++ > reconnectFailTimes )
                {
                    stringstream ss;
                    ss << "Timed out (" << timeoutMs << "ms) waiting for data.  Reconnecting.";
                    context_.tracer().warning( ss.str() );
                    health().warning( ss.str() );
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
                health().ok();
                context_.tracer().debug( "MainThread: can't compute localiseData" );
                continue;
            }

            // copy the timestamp
            localiseData.timeStamp = gpsData.timeStamp;

            localiseInterface_->localSetAndSend( localiseData );

            health().ok();

        } // try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );
        }
    } // while
}

///////////////////////////

void 
MainThread::initDriver( orca::VehicleDescription vehicleDescr )
{
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();
    std::string driverName = orcaice::getPropertyWithDefault( prop,
            prefix+"Driver", "simple" );
    
    if ( driverName == "simple" )
    {
        orca::GpsDescription gpsDescr = getGpsDescription();

        context_.tracer().debug( "loading 'simple' driver",3);
        driver_ = new SimpleDriver( gpsDescr, vehicleDescr, context_ );
        context_.tracer().debug( "driver loaded OK.",3);
    }
    else if ( driverName == "odometrybased" )
    {
        orca::GpsDescription gpsDescr = getGpsDescription();

        context_.tracer().debug( "loading 'odometrybased' driver",3);
        driver_ = new OdometryBasedDriver( gpsDescr, vehicleDescr, context_ );
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

orca::GpsDescription
MainThread::getGpsDescription()
{
    orca::GpsDescription gpsDescr;

    while ( !isStopping() )
    {
        try
        {    
            orca::GpsPrx gpsPrx;
            orcaice::connectToInterfaceWithTag<orca::GpsPrx>( context_, gpsPrx, "Gps" );

            context_.tracer().debug( "Getting gps description...", 2 );
            gpsDescr = gpsPrx->getDescription();
            stringstream ss;
            ss << "Got gps description: " << ifaceutil::toString( gpsDescr );
            context_.tracer().info( ss.str() );

            checkAntennaOffsetOK( gpsDescr.antennaOffset );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "getting Gps description", health(), gbxutilacfr::SubsystemFault, 2000 );
        }
    }
    return gpsDescr;
}
