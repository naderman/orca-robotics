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
#include <orcaice/proputils.h>

#include "component.h"
#include "imuhandler.h"

// Various bits of hardware we can drive
#include "imudriver.h"
#include "fakeimudriver.h"
// #include "honeywell/honeywelldriver.h"

// implementations of Ice objects
#include "imuI.h"
// #include <orcaifaceimpl/odometry3dImpl.h>

#ifdef HAVE_PLAYERCLIENT_DRIVER
#  include "playerclientodometry3d/playerclientodometry3ddriver.h"
#endif

using namespace std;
using namespace orca;

namespace imu {

Component::Component()
    : orcaice::Component( "Imu" ),
      handler_(0),
      hwDriver_(0)
{
}

Component::~Component()
{
    delete hwDriver_;
    // do not delete the handler_, it's a Thread and self-distructs.
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag() + ".Config.";

    bool startEnabled = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );
    
    orca::ImuDescription imuDescr;
    orca::VehicleDescription odom3dDescr;

    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine imu type.  Expected property '";
        errString += prefix + "Driver'";
        throw orcaiceutil::Exception( ERROR_INFO, errString );
    }

    orca::CartesianPoint defaultOffset;
    defaultOffset.x = defaultOffset.y = defaultOffset.z = 0;
    orca::CartesianPoint frameOffset = orcaice::getPropertyAsCartesianPointWithDefault(
			prop, prefix+"frameOffset", defaultOffset);

    //
    // HARDWARE INTERFACES
    //

    
/*    if ( driverName == "honeywell" )
    {
        std::string device = orcaice::getPropertyWithDefault( prop, prefix+"Device", "/dev/ttyS0" );
        int baud = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baud", 4800 );

        hwDriver_ = new HoneywellImuDriver( device.c_str(), baud);
    }*/
    if ( driverName == "fake" )
    {
        hwDriver_ = new FakeImuDriver;
    }
    else if ( driverName == "playerclientodometry3d" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER        
        std::string playerHost = orcaice::getPropertyWithDefault(      prop, prefix+"PlayerClient.Host", "localhost" );
        int         playerPort = orcaice::getPropertyAsIntWithDefault( prop, prefix+"PlayerClient.Port", 6665 );
        // int         playerDev  = orcaice::getPropertyAsIntWithDefault( prop, prefix+"PlayerClient.Device", 0 );
        hwDriver_ = new PlayerClientOdometry3dDriver( playerHost.c_str(), playerPort, false );
#else
        throw orcaiceutil::Exception( ERROR_INFO, "Can't instantiate driverName 'playerclientodometry3d' without player install!" );
#endif
    }

    else
    {
        std::string errString = "unknown imu type: "+driverName;
        context().tracer()->error( errString );
        throw orcaiceutil::Exception( ERROR_INFO, errString );
        return;
    }

    if(hwDriver_->reset()<0){
        std::string errString = "Failed to reset IMU.";
        context().tracer()->error( errString );
        throw orcaiceutil::Exception( ERROR_INFO, errString );
        return;
    }

    hwDriver_->enable();

    // wait until we have a fix before publishing etc.
    /*
    do{
        ret=hwDriver_->read();
        if(ret==-1){
            std::string errString = "Failed to read from IMU.";
            context().tracer()->error( errString );
            throw orcaiceutil::Exception( ERROR_INFO, errString );
            return;
        }
        if(!isActive()){
            return;
        }
    }while(!hwDriver_->hasFix());
    */

    // create servant for direct connections
    ImuI* imuObj = new ImuI( imuDescr, context() );
    imuObjPtr_ = imuObj;
    orcaice::createInterfaceWithTag( context(), imuObjPtr_, "Imu" );
    
    // Position3dI* position3dObj_ = new Position3dI( pos3dDescr, context() );
    // position3dObjPtr_ = position3dObj_;
    // orcaice::createInterfaceWithTag( context(), position3dObjPtr_, "Position3d" );

    //
    // PROVIDED: Odometry3d
    //
    odometry3dImpl_ = 
        new orcaifaceimpl::Odometry3dImpl( odom3dDescr, "Odometry3d", context() );


    while ( true ) // isActive() 
    { 
        try {
            odometry3dImpl_->initInterface();
            context().tracer()->debug( "odometry interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            context().tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        catch ( const Ice::Exception& e ) {
            context().tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }


    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    context().tracer()->debug( "entering handler_...",5 );

    handler_ = new ImuHandler(*imuObj,
                              *odometry3dImpl_,
                              hwDriver_,
                              frameOffset,
                              context(),
                              startEnabled );

    handler_->start();
}

void 
Component::stop()
{
    tracer()->info("stopping component...");
    orcaiceutil::stopAndJoin( handler_ );
}

} //namespace
