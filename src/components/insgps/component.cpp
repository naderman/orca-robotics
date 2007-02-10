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
    odometry3dHandler_(0)
{
}

Component::~Component()
{
    // do not delete the gpsObj_, imuObj_, or odometry3dObj_ as they are smart pointers and self destruct
    // do not delete gpsHandler_, imuHandler_, or odometry3dHandler_, or the hwDriver_
    // as they are orcaice::Threads and self-destruct.
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
        // TODO: need to set this at a baud rate of 230400
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
    // vehicleDescr.timeStamp = orcaice::getNow();

    //
    // transfer internal sensor configs
    //
    
    // offset from global coordinate system
    orcaice::setInit( gpsDescr.offset );
    gpsDescr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Gps.Offset", gpsDescr.offset );

    orcaice::setInit( gpsDescr.size );
    gpsDescr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Gps.Size", gpsDescr.size );
    
    // offset from robot coordinate system
    orcaice::setInit( imuDescr.offset );
    imuDescr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Imu.Offset", imuDescr.offset );
   
    orcaice::setInit( imuDescr.size );
    imuDescr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Imu.Size", imuDescr.size );

    // offset from robot coordinate system
    // orcaice::setInit( vehicleDescr.offset );
    // vehicleDescr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Odometry3d.Offset", vehicleDescr.offset );
   
    // orcaice::setInit( vehicleDescr.size );
    // vehicleDescr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Odometry3d.Size", imuDescr.size );
    
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
    orcaice::Thread::stopAndJoin( gpsHandler_ );
    orcaice::Thread::stopAndJoin( imuHandler_ );
    orcaice::Thread::stopAndJoin( odometry3dHandler_ );   
    orcaice::Thread::stopAndJoin( localise3dHandler_ );   
    // tracer()->debug( "stopped handlers", 2 );

    tracer()->debug( "stopping driver", 2 );
    orcaice::Thread::stopAndJoin( hwDriver_ );
    // tracer()->debug( "stopped driver", 2 );
    
    tracer()->debug( "stopped component", 2 );
}

} //namespace
