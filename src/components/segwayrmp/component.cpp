/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcamisc/configutils.h>
#include <orca/vehicledescription.h>

#include "component.h"
#include "nethandler.h"
#include "orcarobotdriverutil/hwdriverhandler.h"

// segway rmp drivers
#include "fakedriver.h"
#include "rmpdriver/rmpdriver.h"
#ifdef HAVE_USB_DRIVER
#   include "rmpdriver/usb/rmpusbioftdi.h"
#endif
#ifdef HAVE_CAN_DRIVER
#   include "rmpdriver/can/peakcandriver.h"
#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
#   include "playerclient/playerclientdriver.h"
#endif

using namespace std;

namespace segwayrmp {

Component::Component() :
    orcaice::Component( "SegwayRmp" ),
    netHandler_(0),
    hwHandler_(0)
{
}

Component::~Component()
{
    // do not delete handlers!!! They derive from Ice::Thread and self-destruct.
}

orca::VehicleDescription
Component::loadDriver()
{
    //
    // Read vehicle description from config file
    // This VehicleDescription is interpreted as "user preferences". After the hardware
    // in started, it can be modified to reflect actual physical limits of the robot.
    //
    orca::VehicleDescription descr;
    std::string prefix = context().tag() + ".Config.";
    orcamisc::readVehicleDescription( context().properties(), prefix, descr );
    stringstream ss;
    ss<<"TRACE(component.cpp): Read vehicle description from configuration: " 
        << endl << orcaice::toString(descr) << endl;
    context().tracer()->info( ss.str() );

    //
    // Check vehicle desciption sanity
    //
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw orcaiceutil::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw orcaiceutil::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( 
            context().properties(), prefix+"Driver", "segwayrmpusb" );

    if ( driverName == "segwayrmpusb" || driverName == "segwayrmpcan" )
    {
        if ( driverName == "segwayrmpusb" )
        {
#ifdef HAVE_USB_DRIVER
            context().tracer()->debug( "HwHandler: loading 'segwayrmpusb' driver",3);
            rmpIo_.reset( new RmpUsbIoFtdi );
#else
            throw orcaiceutil::Exception( ERROR_INFO, "HwHandler: Can't instantiate driver 'usb' because it was not built!" );
#endif            
        }
        else if ( driverName == "segwayrmpcan" )
        {
#ifdef HAVE_CAN_DRIVER
        context().tracer()->debug( "HwHandler: loading 'peakcan' driver",3);
        
        //Get the port name that we are being asked to open
        string portName;
        if( orcaice::getProperty( context().properties(), prefix+"SegwayRmpCan.PortName", portName ) !=0 ){
            throw orcaiceutil::Exception( ERROR_INFO, "HwHandler::HwHandler() Config.SegwayRmpCan.PortName not specified" );
        }

        rmpIo_.reset( new PeakCanDriver ( portName ) );
#else
        throw orcaiceutil::Exception( ERROR_INFO, "HwHandler: Can't instantiate driver 'peakcan' because it was not built!" );
#endif            
        }
        else { assert(false); }

        RmpDriver *rmpDriver = new RmpDriver( context(), *rmpIo_ );
        driver_.reset( rmpDriver );

        //
        // Checks that the description matches what this hardware can actually do.
        // This may change values inside the description structure!
        //
        rmpDriver->applyHardwareLimits( controlDescr->maxForwardSpeed, 
                                        controlDescr->maxReverseSpeed,
                                        controlDescr->maxTurnrate,
                                        controlDescr->maxTurnrateAtMaxSpeed );
    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context().tracer()->debug( "HwHandler: loading 'playerclient' driver",3);
        driver_.reset( new PlayerClientDriver( context() ) );
#else
        throw orcaiceutil::Exception( ERROR_INFO, "HwHandler: Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context().tracer()->debug( "HwHandler: loading 'fake' driver",3);
        driver_.reset( new FakeDriver( context() ) );
    }
    else {
        string errorStr = "HwHandler: Unknown driver type. Cannot talk to hardware.";
        context().tracer()->error( errorStr);
        context().tracer()->info( "HwHandler: Valid driver values are {'segwayrmpcan', 'segwayrmpusb', 'playerclient', 'fake'}" );
        throw orcaiceutil::Exception( ERROR_INFO, errorStr );
    }

    return descr;
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    tracer()->debug( "Starting Component",2 );

    // 
    // Read vehicle description and load driver
    //
    orca::VehicleDescription descr = loadDriver();

    //
    // Hardware handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    bool isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( context().properties(),
                                                                       context().tag()+".Config.EnableMotion",
                                                                       1 );
    orcarobotdriverutil::HwDriverHandler<Command,Data> *hwHandler = 
        new orcarobotdriverutil::HwDriverHandler<Command,Data>( *driver_,
                                                                isMotionEnabled,
                                                                context() );
    hwHandler_ = hwHandler;
    hwHandler_->start();

    //
    // Network handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    netHandler_ = new NetHandler( *hwHandler, descr, context() );
    // this thread will try to activate and register the adapter
    netHandler_->start();

    // the rest is handled by the application/service
    context().tracer()->debug( "Component::start() done." );
}

void
Component::stop()
{
    tracer()->debug( "stopping component", 2 );
    orcaiceutil::stopAndJoin( netHandler_ );
    tracer()->info( "stopped net handler", 2 );
    orcaiceutil::stopAndJoin( hwHandler_ );
    tracer()->info( "stopped hw handler", 2 );
}

}
