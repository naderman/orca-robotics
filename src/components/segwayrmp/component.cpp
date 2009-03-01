/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaobjutil/vehicleutil.h>

#include "component.h"
#include "netthread.h"
#include "hwthread.h"

using namespace std;

namespace segwayrmp {

//////////////////////////////////////////////////////////////

namespace {

void convert( const orca::VehicleControlVelocityDifferentialDescription& descr, HwThread::Config& config )
{
    config.maxForwardSpeed = descr.maxForwardSpeed;
    config.maxReverseSpeed = descr.maxReverseSpeed;
    config.maxTurnrate     = descr.maxTurnrate;
    config.maxForwardAcceleration = descr.maxForwardAcceleration;
    config.maxReverseAcceleration = descr.maxReverseAcceleration;
    config.maxLateralAcceleration = descr.maxLateralAcceleration;
}

void convert( const HwThread::Config& config, orca::VehicleControlVelocityDifferentialDescription& descr )
{
    descr.maxForwardSpeed = config.maxForwardSpeed;
    descr.maxReverseSpeed = config.maxReverseSpeed;
    descr.maxTurnrate     = config.maxTurnrate;
    descr.maxForwardAcceleration = config.maxForwardAcceleration;
    descr.maxReverseAcceleration = config.maxReverseAcceleration;
    descr.maxLateralAcceleration = config.maxLateralAcceleration;
}

}

//////////////////////////////////////////////////////////////


Component::Component() :
    orcaice::Component( "SegwayRmp" )
{
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
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
        << endl << orcaobj::toString(descr) << endl;
    context().tracer().info( ss.str() );

    //
    // Check vehicle desciption sanity
    //
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw gbxutilacfr::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw gbxutilacfr::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    //
    // Hardware handling loop
    //
    HwThread::Config checkedConfig;
    convert( *controlDescr, checkedConfig );
    //
    // Checks that the description matches what this hardware can actually do.
    // This may change values inside the description structure!
    //
    HwThread *hwThread = new HwThread( checkedConfig, context() );
    convert( checkedConfig, *controlDescr );

    // this thread will talk to the hardware
    hwThread_ = hwThread;
    hwThread_->start();

    //
    // Network handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    netThread_ = new NetThread( *hwThread, descr, context() );

    // this thread will try to activate and register the adapter
    netThread_->start();

    // give MainThread a chance to write something to history
    context().history().autoStart( false );
    context().history().flush();

    // the rest is handled by the application/service
    context().tracer().debug( "Component::start() done." );
}

void
Component::stop()
{
    context().tracer().debug( "stopping component", 2 );
    gbxiceutilacfr::stopAndJoin( netThread_ );
    context().tracer().info( "stopped net handler", 2 );
    gbxiceutilacfr::stopAndJoin( hwThread_ );
    context().tracer().info( "stopped hw handler", 2 );

    // give MainThread a chance to write something to history
    context().history().autoFinish( false );
    context().history().flush();
}

} // namespace
