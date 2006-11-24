/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "component.h"

// Various bits of hardware we can drive
#include "driver.h"
#include "fakeinsgpsdriver.h"
#include "novatelspan/novatelspandriver.h"

using namespace std;
using namespace orca;

namespace insgps{

Component::Component()
    : orcaice::Component( "InsGps" ),
      hwDriver_(0)
{

}

Component::~Component()
{
    delete hwDriver_;
    
    // do not delete the gpsObj_, imuObj_, or position3dObj_ as they are orcaice::Threads and self-distruct.
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

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

    //
    // HARDWARE INTERFACES
    //

    
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

    hwDriver_->init();
   
    // query driver for the actual configuration after initialization
    Driver::Config actualCfg = hwDriver_->config();

     
    //
    // SENSOR DESCRIPTION
    //

    orca::GpsDescriptionPtr gpsDescr = new orca::GpsDescription;
    gpsDescr->timeStamp = orcaice::getNow();

    orca::ImuDescriptionPtr imuDescr = new orca::ImuDescription;
    imuDescr->timeStamp = orcaice::getNow();
    
    orca::Position3dDescriptionPtr position3dDescr = new orca::Position3dDescription;
    position3dDescr->timeStamp = orcaice::getNow();

    //
    // transfer internal sensor configs
    //
    
    // offset from global coordinate system
    orcaice::setInit( gpsDescr->offset );
    gpsDescr->offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Gps.Offset", gpsDescr->offset );

    orcaice::setInit( gpsDescr->size );
    gpsDescr->size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Gps.Size", gpsDescr->size );
    
    // offset from robot coordinate system
    orcaice::setInit( imuDescr->offset );
    imuDescr->offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Imu.Offset", imuDescr->offset );
   
    orcaice::setInit( imuDescr->size );
    imuDescr->size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Imu.Size", imuDescr->size );

    // offset from robot coordinate system
    orcaice::setInit( position3dDescr->offset );
    position3dDescr->offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Position3d.Offset", position3dDescr->offset );
   
    orcaice::setInit( position3dDescr->size );
    position3dDescr->size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Position3d.Size", imuDescr->size );
    
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
    // Each of these interface implementations are separate threads which read from the hwDriver_'s buffers
    // and checks if the corresponding message is available. If it is, the message is popped
    // from the buffer and published to the outside world.
    gpsObj_ = new GpsI( gpsDescr, hwDriver_, context() );
    imuObj_ = new ImuI( imuDescr, hwDriver_, context() );
    position3dObj_ = new Position3dI( position3dDescr, hwDriver_, context() );

    // create smart pointers for each of the objects   
    gpsObjPtr_ = gpsObj_;
    imuObjPtr_ = imuObj_;
    position3dObjPtr_ = position3dObj_;

    // register each of the objects so that remote calls know that these things exist   
    orcaice::createInterfaceWithTag( context(), gpsObjPtr_, "Gps" );
    orcaice::createInterfaceWithTag( context(), imuObjPtr_, "Imu" );
    orcaice::createInterfaceWithTag( context(), position3dObjPtr_, "Position3d" );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    context().tracer()->debug( "entering handlers_...", 5 );

    // now that each of the objects have been registered, we can start their threads
    gpsObj_->start();
    imuObj_->start();
    position3dObj_->start();


}

void Component::stop()
{
    tracer()->debug( "stopping component", 5 );
    orcaice::Thread::stopAndJoin( gpsObj_ );
    orcaice::Thread::stopAndJoin( imuObj_ );
    orcaice::Thread::stopAndJoin( position3dObj_ );
    hwDriver_->shutdown();
    tracer()->debug( "stopped component", 5 );
}

} //namespace
