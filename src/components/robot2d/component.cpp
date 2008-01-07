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
#include <orcaobjutil/vehicleutil.h>

#include "component.h"
#include "netthread.h"
#include "hwthread.h"

using namespace std;
using namespace robot2d;

Component::Component() :
    orcaice::Component( "Robot2d" )
{
}

void
Component::start()
{
    tracer().debug( "Starting Component",2 );

    //
    // Read vehicle description from config file
    // This VehicleDescription is interpreted as "user preferences". After the hardware
    // in started, it can be modified to reflect actual physical limits of the robot.
    //
    orca::VehicleDescription descr;
    std::string prefix = context().tag() + ".Config.";
    orcaobjutil::readVehicleDescription( context().properties(), prefix, descr );
    stringstream ss;
    ss<<"TRACE(component.cpp): Read vehicle description from configuration: " 
        << endl << orcaice::toString(descr) << endl;
    context().tracer().info( ss.str() );

    //
    // Hardware handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    HwThread* hwThread = new HwThread( context() );
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
    context().tracer().debug( "Component::start() done." );
}

void
Component::stop()
{
    tracer().debug( "stopping component", 2 );
    hydroiceutil::stopAndJoin( NetThread_ );
    tracer().info( "stopped net handler", 2 );
    hydroiceutil::stopAndJoin( HwThread_ );
    tracer().info( "stopped hw handler", 2 );
}
