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
#include <sstream>

#include <orcaice/orcaice.h>
#include "netthread.h"
#include "estopconsumerI.h"

using namespace std;
using namespace segwayrmp;

//////////////////////////////////////////////////////////////////////

namespace {
    const char *SUBSYSTEM_NAME = "network";

void 
convert( const hydrointerfaces::SegwayRmp::Data& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.yaw;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = 0.0;
    network.motion.w = internal.dyaw;
}

void 
convert( const hydrointerfaces::SegwayRmp::Data& internal, orca::Odometry3dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.p.z = 0.0;

    network.pose.o.r = internal.roll;
    network.pose.o.p = internal.pitch;
    network.pose.o.y = internal.yaw;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = 0.0;
    network.motion.v.z = 0.0;

    network.motion.w.x = internal.droll;
    network.motion.w.y = internal.dpitch;
    network.motion.w.z = internal.dyaw;
}

// The CU batteries are the main ones.
double
cuBatteryPercentRemaining( double cuVoltage )
{
    const double vMax  = 78;
    const double vMin  = 69;
    const double slope = 100.0 / (vMax-vMin);
    const double b = -vMin * slope;

    double pct = slope * cuVoltage + b;
    return pct;
}

void 
convert( const hydrointerfaces::SegwayRmp::Data& internal, orca::PowerData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    // set up data structure for 2 batteries
    network.batteries.resize(2);
    network.batteries[0].name = "main";
    network.batteries[0].voltage = internal.mainvolt;
    network.batteries[0].percent = cuBatteryPercentRemaining( internal.mainvolt );
    network.batteries[0].isBatteryCharging = orca::ChargingUnknown;
    network.batteries[0].secRemaining      = 0;

    network.batteries[1].name = "ui";
    network.batteries[1].voltage = internal.uivolt;
    network.batteries[1].percent = 0;
    network.batteries[1].isBatteryCharging = orca::ChargingUnknown;
    network.batteries[1].secRemaining = 0;
}

void 
convert( const orca::VelocityControl2dData& network, hydrointerfaces::SegwayRmp::Command& internal )
{
    internal.vx = network.motion.v.x;
    internal.w = network.motion.w;
}

}

//////////////////////////////////////////////////////////////////////

NetThread::NetThread( HwThread                      &hwThread,
                        const orca::VehicleDescription &descr,
                        const orcaice::Context         &context )
    : eStopPrx_(0),
      hwThread_(hwThread),
      descr_(descr),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM_NAME, 10.0 );
    context_.status()->initialising( SUBSYSTEM_NAME );

    // Get vehicle limits
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw hydroutil::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw hydroutil::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    // for symetric limits, only need to store 2 constants.
    maxSpeed_    = controlDescr->maxForwardSpeed;
    maxTurnrate_ = controlDescr->maxTurnrate;

    // By default the estop interface is not enabled...
    isEStopEnabled_ = (bool)orcaice::getPropertyAsIntWithDefault( context_.properties(),
            context_.tag()+".Config.EnableEStopInterface", 0 );
    stringstream ss; ss <<"NetThread: isEStopInterfaceEnabled is set to "<< isEStopEnabled_<<endl;
    context_.tracer()->info( ss.str() );
}

void
NetThread::limit( hydrointerfaces::SegwayRmp::Command &command )
{
    if ( command.vx > maxSpeed_ ) {
        command.vx = maxSpeed_;
        // debug
//         stringstream ss;
//         ss<<"Forward speed limited to "<<command.vx;
//         context_.tracer()->debug(ss.str(),6);
    }
    if ( command.vx < -maxSpeed_ )
        command.vx = -maxSpeed_;

    if ( command.w > maxTurnrate_ )
        command.w = maxTurnrate_;
    if ( command.w < -maxTurnrate_ )
        command.w = -maxTurnrate_;
}

// This is a direct callback from the VelocityControl2dImpl object.
// It's executed in Ice thread.
void 
NetThread::handleData(const orca::VelocityControl2dData& incomingCommand)
{
    hydrointerfaces::SegwayRmp::Command internalCommand;
    convert( incomingCommand, internalCommand );
    limit( internalCommand );
    hwThread_.setCommand( internalCommand );
}


void
NetThread::walk()
{
    activate( context_, this );
    
    std::string prefix = context_.tag() + ".Config.";

    // Initialise external interfaces
    odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr_, "Odometry2d", context_ );
    odometry2dI_->initInterface( this );
    
    odometry3dI_ = new orcaifaceimpl::Odometry3dImpl( descr_, "Odometry3d", context_ );
    odometry3dI_->initInterface( this );

    powerI_ = new orcaifaceimpl::PowerImpl( "Power", context_ );
    powerI_->initInterface( this );

    velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( descr_, "VelocityControl2d", context_ );
    velocityControl2dI_->initInterface( this );
    // register ourselves as data handlers (it will call the handleData() callback).
    velocityControl2dI_->setNotifyHandler( this );

    // Are we using the EStop interface?
    if( isEStopEnabled_ ){
        initEStopCallback();    
    }

    // temp objects in network format
    orca::Odometry2dData odometry2dData;
    orca::Odometry3dData odometry3dData;
    orca::PowerData      powerData;

    hydroutil::Timer odometry2dPublishTimer;
    hydroutil::Timer odometry3dPublishTimer;
    hydroutil::Timer powerPublishTimer;
    hydroutil::Timer eStopDataLastRxTimer;

    double odometry2dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"Odometry2dPublishInterval", 0.1 );
    double odometry3dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"Odometry3dPublishInterval", 0.1 );
    double powerPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"PowerPublishInterval", 20.0 );

    const int odometryReadTimeout = 500; // [ms]
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM_NAME, 2.0*(odometryReadTimeout/1000.0) );
    


    //
    // Main loop
    //
    while( !isStopping() )
    {
//         context_.tracer()->debug( "net handler loop spinning ",1);

        // block on the highest frequency incoming data stream
        hydrointerfaces::SegwayRmp::Data data;
        if ( hwThread_.getData( data, odometryReadTimeout ) ) {
//             context_.tracer()->debug( "Net loop timed out", 1);
            // Don't flag this as an error -- it may happen during normal initialisation.
            context_.status()->ok( SUBSYSTEM_NAME, "Net loop timed out" );
            continue;
        }

        // Odometry2d
        convert( data, odometry2dData );
        // check that we were not told to terminate while we were sleeping
        // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
        if ( !isStopping() && odometry2dPublishTimer.elapsed().toSecondsDouble()>=odometry2dPublishInterval ) {
            odometry2dI_->localSetAndSend( odometry2dData );
            odometry2dPublishTimer.restart();
        } 
        else {
            odometry2dI_->localSet( odometry2dData );
        }

        // Odometry3d
        convert( data, odometry3dData );
        if ( !isStopping() && odometry3dPublishTimer.elapsed().toSecondsDouble()>=odometry3dPublishInterval ) {
            odometry3dI_->localSetAndSend( odometry3dData );
            odometry3dPublishTimer.restart();
        } 
        else {
            odometry3dI_->localSet( odometry3dData );
        }

        // Power
        convert( data, powerData );
        if ( !isStopping() && powerPublishTimer.elapsed().toSecondsDouble()>=powerPublishInterval ) {
            powerI_->localSetAndSend( powerData );
            powerPublishTimer.restart();
        } 
        else {
            powerI_->localSet( powerData );
        }

        // subsystem heartbeat
        context_.status()->ok( SUBSYSTEM_NAME );
    } // main loop
}



// TODO modernise this code section.
void 
NetThread::initEStopCallback()
{

    // Keep trying to create the interface until we succeed
    orcaice::connectToInterfaceWithTag<orca::EStopPrx>( context_, eStopPrx_, "HatchMon", this );
            
    // callback object to receive data, and hands it straight the the hwThread thread
    Ice::ObjectPtr consumerObj = new segwayrmp::EStopConsumerI(hwThread_);
    orca::EStopConsumerPrx callbackPrx =
      orcaice::createConsumerInterface<orca::EStopConsumerPrx>( context_, consumerObj );
    
    //
    // Subscribe for EStop data
    //
    // will try forever until the user quits with ctrl-c
    while ( true )
    {
        try
        {
            eStopPrx_->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & )
        {
            context_.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
}
