/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orca/vehicledescription.h>

#include "component.h"
#include "netthread.h"
#include <orcaobjutil/vehicleutil.h>
#include "hwthread.h"

// driver types
#include "fakedriver.h"
#ifdef HAVE_PLAYERCLIENT_DRIVER
#   include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace robot2d;

Component::Component() :
    orcaice::Component( "Robot2d" )
{
}

orca::VehicleDescription
Component::loadDriver()
{
    // 
    // Read vehicle description
    //
    orca::VehicleDescription descr;
    orcaobjutil::readVehicleDescription( context().properties(), context().tag()+".Config.", descr );
    stringstream ss;
    ss<<"TRACE(component.cpp): Read vehicle description from configuration: " 
        << endl << orcaice::toString(descr) << endl;
    context().tracer()->info( ss.str() );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( 
            context().properties(), context().tag()+".Config.Driver", "playerclient" );
            
    if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context().tracer()->debug( "loading Player-Client driver",3);
        driver_.reset( new PlayerClientDriver( context() ) );
#else
        throw hydroutil::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context().tracer()->debug( "loading Fake driver",3);
        driver_.reset( new FakeDriver( context() ) );
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context().tracer()->error( errorStr);
        context().tracer()->info( "Valid driver values are {'playerclient', 'fake'}" );
        throw hydroutil::Exception( ERROR_INFO, errorStr );
    }

    context().tracer()->debug( "Component: driver instantiated", 5 );
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
    HwThread* hwThread = new HwThread( *driver_,
                                          isMotionEnabled,
                                          context() );
    HwThread_ = hwThread;
    HwThread_->start();

    //
    // Network handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    NetThread_ = new NetThread( *hwThread, descr, context() );
    // this thread will try to activate and register the adapter
    NetThread_->start();

    // the rest is handled by the application/service
    context().tracer()->debug( "Component::start() done." );
}

void
Component::stop()
{
    tracer()->debug( "stopping component", 2 );
    hydroutil::stopAndJoin( NetThread_ );
    tracer()->info( "stopped net handler", 2 );
    hydroutil::stopAndJoin( HwThread_ );
    tracer()->info( "stopped hw handler", 2 );
}
