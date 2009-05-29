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
#include <sstream>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <orcaice/orcaice.h>
#include <orcaobj/velocitycontrol2d.h>
#include "netthread.h"

using namespace std;

namespace robot2d {

namespace {

void 
convert( const hydrointerfaces::Robot2d::Data& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.yaw;
    
    network.motion.v.x = internal.vlong;
    network.motion.v.y = internal.vtransverse;
    network.motion.w = internal.dyaw;
}

void 
convert( const orca::VelocityControl2dData& network, hydrointerfaces::Robot2d::Command& internal )
{
    internal.vlong = network.motion.v.x;
    internal.vtransverse = network.motion.v.y;
    internal.dyaw  = network.motion.w;
}

}

//////////////////////////////////////////////////////////////////////

NetThread::NetThread( HwThread                      &HwThread,
                        const orca::VehicleDescription &descr,
                        const orcaice::Context         &context ) :
    SubsystemThread( context.tracer(), context.status(), "NetThread" ),
    HwThread_(HwThread),
    descr_(descr),
    context_(context)
{

    // Get vehicle limits
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw gbxutilacfr::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw gbxutilacfr::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    maxSpeed_    = controlDescr->maxForwardSpeed;
    maxTurnrate_ = controlDescr->maxTurnrate;
}

void 
NetThread::initialise()
{
    setMaxHeartbeatInterval( 10.0 );

    // Initialise external interfaces, multi-try init functions
    odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr_, "Odometry2d", context_ );
    odometry2dI_->initInterface( this );

    velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( descr_, "VelocityControl2d", context_ );
    velocityControl2dI_->initInterface( this );
    velocityControl2dI_->setNotifyHandler( this );
}

void
NetThread::work()
{
    // temp objects in internal format
    hydrointerfaces::Robot2d::Data data;

    // temp objects in network format
    orca::Odometry2dData odometry2dData;

    std::string prefix = context_.tag() + ".Config.";

    gbxiceutilacfr::Timer publishTimer;
    double publishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"Odometry2dPublishInterval", 0 );

    const int odometryReadTimeout = 500; // [ms]
    setMaxHeartbeatInterval( 2.0*(odometryReadTimeout/1000.0) );

    //
    // Main loop
    //
    while( !isStopping() )
    {
        // context_.tracer().debug( "NetThread: loop spinning ",9);

        // block on the most frequent data source: odometry
        if ( HwThread_.getData( data, odometryReadTimeout ) ) {
            context_.tracer().debug( "Net loop timed out", 5);
            // Don't flag this as an error -- it may happen during normal initialisation.
            health().ok( "Net loop timed out" );
            continue;
        }

        // Odometry2d
        // convert internal to network format
        convert( data, odometry2dData );
        // check that we were not told to terminate while we were sleeping
        // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
        if ( !isStopping() && publishTimer.elapsed().toSecondsDouble()>=publishInterval ) {
            odometry2dI_->localSetAndSend( odometry2dData );
            publishTimer.restart();
        } 
        else {
            odometry2dI_->localSet( odometry2dData );
        }

        // subsystem heartbeat
        health().ok();
    } // main loop
}

///////////////////

void
NetThread::limit( hydrointerfaces::Robot2d::Command &cmd )
{
    if ( cmd.vlong > maxSpeed_ )
        cmd.vlong = maxSpeed_;
    if ( cmd.vlong < -maxSpeed_ )
        cmd.vlong = -maxSpeed_;

    if ( cmd.vtransverse > maxSpeed_ )
        cmd.vtransverse = maxSpeed_;
    if ( cmd.vtransverse < -maxSpeed_ )
        cmd.vtransverse = -maxSpeed_;

    if ( cmd.dyaw > maxTurnrate_ )
        cmd.dyaw = maxTurnrate_;
    if ( cmd.dyaw < -maxTurnrate_ )
        cmd.dyaw = -maxTurnrate_;
}

// This is a direct callback from the VelocityControl2dImpl object.
// It's executed in Ice thread.
// Here we convert to our internal format pass it to HwThread
void 
NetThread::handleData(const orca::VelocityControl2dData& command)
{
    stringstream ss;
    ss << "NetThread::handleData: " << orcaobj::toString(command);
    context_.tracer().debug( ss.str() );

    try {
        hydrointerfaces::Robot2d::Command internalCommand;
        convert( command, internalCommand );
        limit( internalCommand );
        HwThread_.setCommand( internalCommand );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss<<"NetThread::handleData() Caught unexpected exception: " << e.what() << endl;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( ... )
    {
        context_.tracer().error( "NetThread::handleData(): Caught unexpected unknown exception." );
        throw;
    }
}

}
