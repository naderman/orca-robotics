/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <fstream>

#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "netthread.h"

using namespace std;
using namespace segwayrmp;

//////////////////////////////////////////////////////////////////////

namespace {

void 
saveToFile( const hydrointerfaces::SegwayRmp::Data &data, 
            const std::string &fname )
{
    std::ofstream logfile( fname.c_str(), ios::app );
    if ( !logfile.is_open() ) 
    {
        stringstream ss; ss << "Failed to open file for writing: " << fname;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    logfile << data.seconds << " " 
            << data.useconds << " "
            << data.x << " "
            << data.y << " "
            << data.vx << " "
            << data.leftTorque << " "
            << data.rightTorque << " "
            << data.mainvolt << " "
            << data.uivolt << endl;

    logfile.close();
}


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
    network.batteries[1].percent = 100;
    network.batteries[1].isBatteryCharging = orca::ChargingUnknown;
    network.batteries[1].secRemaining = 1e6;
}

void 
convert( const orca::VelocityControl2dData& network, hydrointerfaces::SegwayRmp::Command& internal )
{
    internal.vx = network.motion.v.x;
    internal.w = network.motion.w;
}

bool
commandPossible( double cmdSpeed,
                 double cmdTurnrate,
                 double maxForwardSpeed,
                 double maxReverseSpeed,
                 double maxTurnrate,
                 double maxLateralAcceleration )
{
    if ( cmdSpeed > maxForwardSpeed ) return false;
    // Note that maxReverseSpeed is a positive number.
    if ( cmdSpeed < -maxReverseSpeed ) return false;
    if ( cmdTurnrate > maxTurnrate ) return false;
    if ( cmdTurnrate < -maxTurnrate ) return false;

    double lateralAcceleration = cmdSpeed*cmdTurnrate;
    if ( lateralAcceleration > maxLateralAcceleration ) return false;

    return true;
}

void
limit( double &cmdSpeed,
       double &cmdTurnrate,
       double maxForwardSpeed,
       double maxReverseSpeed,
       double maxTurnrate,
       double maxLateralAcceleration )
{
    // Note that maxReverseSpeed is a positive number.
    CLIP_TO_LIMITS( -maxReverseSpeed, cmdSpeed, maxForwardSpeed );
    CLIP_TO_LIMITS( -maxTurnrate, cmdTurnrate, maxTurnrate );

    double maxTurnrateToSatisfyLateralAcc = maxLateralAcceleration / fabs(cmdSpeed);
    CLIP_TO_LIMITS( -maxTurnrateToSatisfyLateralAcc, cmdTurnrate, maxTurnrateToSatisfyLateralAcc );
}

}

//////////////////////////////////////////////////////////////////////

NetThread::NetThread( HwThread                      &hwThread,
                        const orca::VehicleDescription &descr,
                        const orcaice::Context         &context ) :
    SubsystemThread( context.tracer(), context.status(), "NetThread" ),
    hwThread_(hwThread),
    descr_(descr),
    context_(context)
{
    subStatus().initialising();
    subStatus().setMaxHeartbeatInterval( 10.0 );

    // Get vehicle limits
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw gbxutilacfr::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw gbxutilacfr::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    maxForwardSpeed_        = controlDescr->maxForwardSpeed;
    maxReverseSpeed_        = controlDescr->maxForwardSpeed;
    maxTurnrate_            = controlDescr->maxTurnrate;
    maxLateralAcceleration_ = controlDescr->maxLateralAcceleration;
}

void 
NetThread::initialise()
{
    // this is a multi-try function to activate component's server capabilities
    activate( context_, this, subsysName() );
    
    std::string prefix = context_.tag() + ".Config.";

    // Initialise external interfaces
    odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr_, "Odometry2d", context_ );
    odometry2dI_->initInterface( this, subsysName() );
    
    odometry3dI_ = new orcaifaceimpl::Odometry3dImpl( descr_, "Odometry3d", context_ );
    odometry3dI_->initInterface( this, subsysName() );

    powerI_ = new orcaifaceimpl::PowerImpl( "Power", context_ );
    powerI_->initInterface( this, subsysName() );

    velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( descr_, "VelocityControl2d", context_ );
    velocityControl2dI_->initInterface( this, subsysName() );
    // register ourselves as data handlers (it will call the handleData() callback).
    velocityControl2dI_->setNotifyHandler( this );
}

void
NetThread::work()
{
    std::string prefix = context_.tag() + ".Config.";

    // temp objects in network format
    orca::Odometry2dData odometry2dData;
    orca::Odometry3dData odometry3dData;
    orca::PowerData      powerData;

    gbxiceutilacfr::Timer odometry2dPublishTimer;
    gbxiceutilacfr::Timer odometry3dPublishTimer;
    gbxiceutilacfr::Timer powerPublishTimer;

    double odometry2dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"Odometry2dPublishInterval", 0.1 );
    double odometry3dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"Odometry3dPublishInterval", 0.1 );
    double powerPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
        context_.properties(), prefix+"PowerPublishInterval", 20.0 );

    const int odometryReadTimeout = 500; // [ms]
    subStatus().setMaxHeartbeatInterval( 2.0*(odometryReadTimeout/1000.0) );
    

    //
    // Main loop
    //
    while( !isStopping() )
    {
//         context_.tracer().debug( "net handler loop spinning ",1);

        // block on the highest frequency incoming data stream
        hydrointerfaces::SegwayRmp::Data data;
        if ( hwThread_.getData( data, odometryReadTimeout ) ) {
//             context_.tracer().debug( "Net loop timed out", 1);
            // Don't flag this as an error -- it may happen during normal initialisation.
            subStatus().ok( "Net loop timed out" );
            continue;
        }

        // save to a text file
        saveToFile( data, "/tmp/segwaybattery.txt" );

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
        subStatus().ok();
    } // main loop
}

/////////////////////////

// This is a direct callback from the VelocityControl2dImpl object.
// It's executed in Ice thread.
void 
NetThread::handleData(const orca::VelocityControl2dData& incomingCommand)
{
    hydrointerfaces::SegwayRmp::Command internalCommand;
    convert( incomingCommand, internalCommand );

    if ( !commandPossible( internalCommand.vx,
                           internalCommand.w,
                           maxForwardSpeed_,
                           maxReverseSpeed_,
                           maxTurnrate_,
                           maxLateralAcceleration_ ) )
    {
        hydrointerfaces::SegwayRmp::Command originalCommand = internalCommand;
        limit( internalCommand.vx,
               internalCommand.w,
               maxForwardSpeed_,
               maxReverseSpeed_,
               maxTurnrate_,
               maxLateralAcceleration_ );

        stringstream ss;
        ss << "Requested command ("<<originalCommand.toString()<<") can not be achieved.  " << endl
           << "  maxForwardSpeed        : " << maxForwardSpeed_ << endl
           << "  maxReverseSpeed        : " << maxReverseSpeed_ << endl
           << "  maxTurnrate            : " << maxTurnrate_*180.0/M_PI << "deg/s" << endl
           << "  maxLateralAcceleration : " << maxLateralAcceleration_ << endl
           << "    --> limiting command to: " << internalCommand.toString();
        subStatus().warning( ss.str() );
    }

    hwThread_.setCommand( internalCommand );
}
