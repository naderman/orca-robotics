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
#include <sstream>

#include <orcaice/orcaice.h>
#include "nethandler.h"

using namespace std;

namespace robot2d {

namespace {

void 
convert( const robot2d::Data& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.o;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = internal.vy;
    network.motion.w = internal.w;
}

void 
convert( const orca::VelocityControl2dData& network, robot2d::Command& internal )
{
    internal.vx = network.motion.v.x;
    internal.vy = network.motion.v.y;
    internal.w  = network.motion.w;
}

}

//////////////////////////////////////////////////////////////////////

NetHandler::NetHandler( orcarobotdriverutil::HwDriverHandler<Command,Data> &hwDriverHandler,
                        const orca::VehicleDescription                     &descr,
                        const orcaice::Context                             &context )
    : hwDriverHandler_(hwDriverHandler),
      descr_(descr),
      context_(context)
{
    // Get vehicle limits
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw orcaice::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw orcaice::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    maxSpeed_    = controlDescr->maxForwardSpeed;
    maxTurnrate_ = controlDescr->maxTurnrate;
}

NetHandler::~NetHandler()
{
}

void
NetHandler::limit( Command &cmd )
{
    if ( cmd.vx > maxSpeed_ )
        cmd.vx = maxSpeed_;
    if ( cmd.vx < -maxSpeed_ )
        cmd.vx = -maxSpeed_;

    if ( cmd.vy > maxSpeed_ )
        cmd.vy = maxSpeed_;
    if ( cmd.vy < -maxSpeed_ )
        cmd.vy = -maxSpeed_;

    if ( cmd.w > maxTurnrate_ )
        cmd.w = maxTurnrate_;
    if ( cmd.w < -maxTurnrate_ )
        cmd.w = -maxTurnrate_;
}

// This is a direct callback from the VelocityControl2dImpl object.
// It's executed in Ice thread.
// Here we convert to our internal format and stick it into
// another Notify pipe to be handled the HwHandler.
void 
NetHandler::handleData(const orca::VelocityControl2dData& command)
{
    stringstream ss;
    ss << "NetHandler::handleData: " << orcaice::toString(command);
    context_.tracer()->debug( ss.str() );

    try {
        robot2d::Command internalCommand;
        convert( command, internalCommand );
        limit( internalCommand );
        hwDriverHandler_.setCommand( internalCommand );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss<<"NetHandler::handleData() Caught unexpected exception: " << e.what() << endl;
        context_.tracer()->error( ss.str() );
        throw;
    }
    catch ( ... )
    {
        context_.tracer()->error( "NetHandler::handleData(): Caught unexpected unknown exception." );
        throw;
    }
}

void
NetHandler::walk()
{
    // multi-try function
    context_.tracer()->debug( "NetHandler: activating..." );
    orcaice::activate( context_, this );
    
    std::string prefix = context_.tag() + ".Config.";
    
    // Initialise external interfaces, multi-try init functions
    odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr_, "Odometry2d", context_ );
    odometry2dI_->initInterface( this );

    velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( descr_, "VelocityControl2d", context_ );
    velocityControl2dI_->initInterface( this );
    velocityControl2dI_->setNotifyHandler( this );

    // temp objects in internal format
    Data data;

    // temp objects in network format
    orca::Odometry2dData odometry2dData;

    orcaice::Timer publishTimer;
    double publishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"Odometry2dPublishInterval", 0 );

    const int odometryReadTimeout = 500; // [ms]

    context_.tracer()->debug( "NetHandler: interface is set up." );

    //
    // Main loop
    //
    while( !isStopping() )
    {
        // context_.tracer()->debug( "NetHandler: loop spinning ",9);

        // block on the most frequent data source: odometry
        if ( hwDriverHandler_.dataProxy().getNext( data, odometryReadTimeout ) ) {
            context_.tracer()->debug( "Net loop timed out", 5);
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
    } // main loop
}

}
