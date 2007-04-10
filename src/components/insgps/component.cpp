/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "component.h"
#include "handler.h"

// Various bits of hardware we can drive
#include "fakeinsgpsdriver.h"
#include "novatelspan/novatelspandriver.h"

using namespace std;

namespace insgps
{

Component::Component() :
    orcaice::Component( "InsGps" ),
    hwDriver_(0),
    gpsHandler_(0),         
    imuHandler_(0),
    odometry3dHandler_(0),
    localise3dHandler_(0)
{
}

Component::~Component()
{
    // Do not delete the gpsObj_, imuObj_, odometry3dObj_, or localise3dObj_
    // as they are smart pointers and self destruct.
    // Do not delete gpsHandler_, imuHandler_, odometry3dHandler_, localise3dHandler_, 
    // or the hwDriver_ as they are orcaice::Threads and self-destruct.
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag() + ".Config.";

    //
    // DRIVER CONFIGURATION
    //

    // read config options
    Driver::Config desiredCfg;

    // Transformation from global (arbitrarily defined) coordinate system (CS) to
    // the GPS MapGrid CS.
    orcaice::setInit( desiredCfg.gpsOffset );
    desiredCfg.gpsOffset = orcaice::getPropertyAsFrame2dWithDefault( prop, prefix+"Gps.Offset", desiredCfg.gpsOffset );

    // Specifies location of the GPS antenna with respect to the vehicles's
    // coordinate system. 
    // Note that this is different from the Novatel.ImuToGpsAntennaOffset which is the lever arm between
    // the imu and antenna, not vehicle and antenna.
    orcaice::setInit( desiredCfg.gpsAntennaOffset );
    desiredCfg.gpsAntennaOffset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Gps.AntennaOffset", desiredCfg.gpsAntennaOffset );
    
    // offset of the imu with respect to the robot's local coordinate system
    orcaice::setInit( desiredCfg.imuOffset );
    desiredCfg.imuOffset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Imu.Offset", desiredCfg.imuOffset );

    /*** now read from config file -RF ***/ 
    // consider the special case of the sensor mounted level (pitch=0) but upside-down (roll=180)
//     desiredCfg.imuFlipped = false;
//     if ( NEAR(desiredCfg.imuOffset.o.r, M_PI, 0.001) && desiredCfg.imuOffset.o.p==0.0 && desiredCfg.imuOffset.o.y==0.0 ) 
//     {
//             // the offset is appropriate
//             desiredCfg.imuFlipped = true;
//             // now remove the roll angle, we'll compensate for it internally
//             desiredCfg.imuOffset.o.r = 0.0;
//             tracer()->info( "the driver will compensate for upside-down mounted sensor" );
//     }

    orcaice::setInit( desiredCfg.imuSize );
    desiredCfg.imuSize = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Imu.Size", desiredCfg.imuSize );

    // Transformation:
    // - from: the platform's coordinate system (eg. origin at the GPS antenna),
    // - to:   the coordinate system of the vehicle (eg. rear axle of the vehicle).
    orcaice::setInit( desiredCfg.vehiclePlatformToVehicleTransform );
    desiredCfg.vehiclePlatformToVehicleTransform = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Vehicle.PlatformToVehicleTransform", desiredCfg.vehiclePlatformToVehicleTransform );
   
    if ( !desiredCfg.validate() ) {
        tracer()->error( "Failed to validate insgps configuration. "+desiredCfg.toString() );
        // this will kill this component
        throw orcaice::Exception( ERROR_INFO, "Failed to validate insgps configuration" );
    }
   
    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine insgps type.  Expected property '";
        errString += prefix + "Driver'";
        throw orcaice::Exception( ERROR_INFO, errString );
    }
 
    if ( driverName == "novatelspan" )
    {
        std::string device = orcaice::getPropertyWithDefault( prop, prefix+"Device", "/dev/ttyS0" );
        int baud = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baud", 115200 );
        hwDriver_ = new NovatelSpanInsGpsDriver( device.c_str(), baud, desiredCfg, context() );
    }
    else if ( driverName == "fake" )
    {
        hwDriver_ = new FakeInsGpsDriver( desiredCfg, context() );
        tracer()->info( "TRACE(component::start()): Using FakeInsGpsDriver" );
    }
    else
    {
        std::string errString = "unknown insgps type: "+driverName;
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }

    if(hwDriver_->reset()<0){
        std::string errString = "Failed to reset InsGps.";
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }

    if ( hwDriver_->init() < 0)
    {      
        std::string errString = "ERROR(component::start()): Failed to initialise InsGps.";
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }
   
    // query driver for the actual configuration after initialization
    Driver::Config actualCfg = hwDriver_->config();

     
    //
    // SENSOR DESCRIPTION
    //

    orca::GpsDescription gpsDescr;
    gpsDescr.timeStamp = orcaice::getNow();

    orca::ImuDescription imuDescr;
    imuDescr.timeStamp = orcaice::getNow();
    
    orca::VehicleDescription vehicleDescr;

    //
    // transfer internal sensor configs
    //
    
    gpsDescr.offset = actualCfg.gpsOffset;
    gpsDescr.antennaOffset = actualCfg.gpsAntennaOffset;
    imuDescr.offset = actualCfg.imuOffset;
    imuDescr.size = actualCfg.imuSize;
    vehicleDescr.platformToVehicleTransform = actualCfg.vehiclePlatformToVehicleTransform;
   
    // wait until we have a fix before publishing etc.
    /*
    do{
        ret=hwDriver_->read();
        if(ret==-1){
            std::string errString = "Failed to read from IMU.";
            context().tracer()->error( errString );
            throw orcaice::Exception( ERROR_INFO, errString );
            return;
        }
        if(!isActive()){
            return;
        }
    }while(!hwDriver_->hasFix());
    */

    // Create objects for each of the interfaces.
    // These objects contain all the functions for publishing data to the outside world
    gpsObj_ = new GpsI( gpsDescr, hwDriver_, context() );
    imuObj_ = new ImuI( imuDescr, hwDriver_, context() );
    odometry3dObj_ = new Odometry3dI( vehicleDescr, hwDriver_, context() );
    localise3dObj_ = new Localise3dI( vehicleDescr, hwDriver_, context() );

    // create smart pointers for each of the objects   
    gpsObjPtr_ = gpsObj_;
    imuObjPtr_ = imuObj_;
    odometry3dObjPtr_ = odometry3dObj_;
    localise3dObjPtr_ = localise3dObj_;

    // register each of the objects so that remote calls know that these things exist   
    orcaice::createInterfaceWithTag( context(), gpsObjPtr_, "Gps" );
    orcaice::createInterfaceWithTag( context(), imuObjPtr_, "Imu" );
    orcaice::createInterfaceWithTag( context(), odometry3dObjPtr_, "Odometry3d" );
    orcaice::createInterfaceWithTag( context(), localise3dObjPtr_, "Localise3d" );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // HANDLERS
    //
    // Each handler is a separate thread waiting for each type of object from the hwDriver_'s buffers.
    // It checks if the corresponding message is available. If it is, the message is popped
    // from the buffer and published to the outside world.
    
    context().tracer()->debug( "entering handlers_...", 2 );

    gpsHandler_ = new Handler( *gpsObj_,
                               hwDriver_,
                               context() );

    imuHandler_ = new Handler( *imuObj_,
                                hwDriver_,
                                context() );
    
    odometry3dHandler_ = new Handler( *odometry3dObj_,
                                hwDriver_,
                                context() );

    localise3dHandler_ = new Handler( *localise3dObj_,
                                hwDriver_,
                                context() );

    // now that each of the objects have been registered, start their handlers
    gpsHandler_->start();
    imuHandler_->start();
    odometry3dHandler_->start();
    localise3dHandler_->start();

}

void 
Component::stop()
{
    tracer()->debug( "stopping component", 2 );

    tracer()->debug( "stopping handlers", 2 );
    orcaice::stopAndJoin( gpsHandler_ );
    orcaice::stopAndJoin( imuHandler_ );
    orcaice::stopAndJoin( odometry3dHandler_ );   
    orcaice::stopAndJoin( localise3dHandler_ );   
    // tracer()->debug( "stopped handlers", 2 );

    tracer()->debug( "stopping driver", 2 );
    orcaice::stopAndJoin( hwDriver_ );
    // tracer()->debug( "stopped driver", 2 );
    
    tracer()->debug( "stopped component", 2 );
}

} //namespace
