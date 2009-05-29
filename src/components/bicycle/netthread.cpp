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
#include <math.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <orcaice/orcaice.h>
#include "netthread.h"

using namespace std;

namespace bicycle {

namespace {

void 
convert( const hydrointerfaces::Bicycle::Data& internal,
	 double wheelbase,
	 double referenceSpeed,
	 double referenceSteerAngle,
	 orca::Odometry2dData& odometry2dData,
	 orca::DriveBicycleData& driveBicycleData )
{
    odometry2dData.timeStamp.seconds = internal.seconds;
    odometry2dData.timeStamp.useconds = internal.useconds;

    odometry2dData.pose.p.x = internal.x;
    odometry2dData.pose.p.y = internal.y;
    odometry2dData.pose.o = internal.yaw;
    
    odometry2dData.motion.v.x = internal.vlong * cos( internal.steerAngle );
    odometry2dData.motion.v.y = 0;
    
    odometry2dData.motion.w =
	internal.vlong * sin( internal.steerAngle ) /
	(2.0 * M_PI * wheelbase );
    
    driveBicycleData.timeStamp.seconds = internal.seconds;
    driveBicycleData.timeStamp.useconds = internal.useconds;

    driveBicycleData.referenceSteerAngle = referenceSteerAngle;
    driveBicycleData.referenceSpeed = referenceSpeed;
    driveBicycleData.currentSteerAngle = internal.steerAngle;
    driveBicycleData.currentSpeed = internal.vlong;
}

void 
convert( const orca::DriveBicycleCommand& network, hydrointerfaces::Bicycle::Command& internal )
{
    internal.vlong = network.speed;
    internal.steerAngle = network.steerAngle;
}

}

//////////////////////////////////////////////////////////////////////

NetThread::NetThread( HwThread                      &HwThread,
                        const orca::VehicleDescription &descr,
                        const orcaice::Context         &context ) :
    SubsystemThread( context.tracer(), context.status(), "NetThread" ),
    HwThread_(HwThread),
    currentSpeed_(0),
    descr_(descr),
    context_(context)
{
    setMaxHeartbeatInterval( 10.0 );

    // Get vehicle limits
    orca::VehicleControlVelocityBicycleDescription* controlDescr =
        dynamic_cast<orca::VehicleControlVelocityBicycleDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw gbxutilacfr::Exception( ERROR_INFO, "Can only deal with bicycle drive vehicles." );

    // Save a copy for later
    controlDescr_ =
	new orca::VehicleControlVelocityBicycleDescription(*controlDescr);
}

void 
NetThread::initialise()
{
    // multi-try function
    context_.tracer().debug( "NetThread: activating..." );
    orcaice::activate( context_, this );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;
    
    // Initialise external interfaces, multi-try init functions
    odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr_, "Odometry2d", context_ );
    odometry2dI_->initInterface( this );

    driveBicycleI_ = new orcaifaceimpl::DriveBicycleImpl( descr_, "DriveBicycle", context_ );
    driveBicycleI_->initInterface( this );
    driveBicycleI_->setNotifyHandler( this );
}

void
NetThread::work()
{
    // temp objects in internal format
    hydrointerfaces::Bicycle::Data data;

    // temp objects in network format
    orca::Odometry2dData odometry2dData;
    orca::DriveBicycleData driveBicycleData;

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
        if ( HwThread_.dataStore().getNext( data, odometryReadTimeout ) ) {
            context_.tracer().debug( "Net loop timed out", 5);
            // Don't flag this as an error -- it may happen during normal initialisation.
            health().ok( "Net loop timed out" );
            continue;
        }

	// retrieve some information of the last sent command
	double referenceSpeed = 0;
	double referenceSteerAngle = 0;
	hydrointerfaces::Bicycle::Command lastCommand;
	if( HwThread_.getLastCommand( lastCommand ) )
	{
	    referenceSpeed = lastCommand.vlong;
	    referenceSteerAngle = lastCommand.steerAngle;
	}

        // Odometry2d and DriveBicycle
        // convert internal to network format
        convert( data, controlDescr_->wheelbase, referenceSpeed,
		 referenceSteerAngle, odometry2dData, driveBicycleData );

        // check that we were not told to terminate while we were sleeping
        // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
        if ( !isStopping() && publishTimer.elapsed().toSecondsDouble()>=publishInterval ) {
            odometry2dI_->localSetAndSend( odometry2dData );
	    driveBicycleI_->localSetAndSend( driveBicycleData );
            publishTimer.restart();
        } 
        else {
            odometry2dI_->localSet( odometry2dData );
        }

	// store a copy of the current speed
	currentSpeed_ = driveBicycleData.currentSpeed;

        // subsystem heartbeat
        health().ok();
    } // main loop
}

///////////////////

void
NetThread::limit( hydrointerfaces::Bicycle::Command &cmd )
{
    if ( cmd.vlong > controlDescr_->maxForwardSpeed )
        cmd.vlong = controlDescr_->maxForwardSpeed;
    if ( cmd.vlong < -controlDescr_->maxReverseSpeed )
        cmd.vlong = -controlDescr_->maxReverseSpeed;

    // calculate the maximum steer angle at current speed (use a linear
    // constraint model, as specified in vehicledescription.ice)
    double f =
	std::min(1.0, fabs(currentSpeed_) / controlDescr_->maxForwardSpeed);
    double currentMaxSteerAngle =
	controlDescr_->maxSteerAngle * (1.0 - f) +
	controlDescr_->maxSteerAngleAtMaxSpeed * f;

    if ( cmd.steerAngle > currentMaxSteerAngle )
        cmd.steerAngle = currentMaxSteerAngle;
    if ( cmd.steerAngle < -currentMaxSteerAngle )
        cmd.steerAngle = -currentMaxSteerAngle;
}

// This is a direct callback from the DriveBicycleImpl object.
// It's executed in Ice thread.
// Here we convert to our internal format pass it to HwThread
void 
NetThread::handleData(const orca::DriveBicycleCommand& command)
{
    stringstream ss;
    ss << "NetThread::handleData: " << ifaceutil::toString(command);
    context_.tracer().debug( ss.str() );

    try {
        hydrointerfaces::Bicycle::Command internalCommand;
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
