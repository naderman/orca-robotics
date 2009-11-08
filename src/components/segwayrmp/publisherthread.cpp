#include "publisherthread.h"
#include <iostream>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

namespace segwayrmp {

namespace {

    const double RMP_3D_HEIGHT = 0.35;

    void
    convert( const hydrointerfaces::SegwayRmp::Data& internal,
             orca::Odometry2dData&                   network,
             bool                                    odometryWasReset )
    {
        network.timeStamp.seconds = internal.seconds;
        network.timeStamp.useconds = internal.useconds;

        network.pose.p.x = internal.x;
        network.pose.p.y = internal.y;
        network.pose.o = internal.yaw;

        network.motion.v.x = internal.vx;
        network.motion.v.y = 0.0;
        network.motion.w = internal.dyaw;

        network.odometryWasReset = odometryWasReset;
    }

    void
    convert( const hydrointerfaces::SegwayRmp::Data& internal,
             orca::Odometry3dData&                   network,
             bool                                    odometryWasReset )
    {
        network.timeStamp.seconds = internal.seconds;
        network.timeStamp.useconds = internal.useconds;

        network.pose.p.x = internal.x;
        network.pose.p.y = internal.y;
        network.pose.p.z = RMP_3D_HEIGHT;

        network.pose.o.r = internal.roll;
        network.pose.o.p = internal.pitch;
        network.pose.o.y = internal.yaw;

        network.motion.v.x = internal.vx;
        network.motion.v.y = 0.0;
        network.motion.v.z = 0.0;

        network.motion.w.x = internal.droll;
        network.motion.w.y = internal.dpitch;
        network.motion.w.z = internal.dyaw;

        network.odometryWasReset = odometryWasReset;
    }

    // The CU batteries are the main ones.
    int
    cuBatteryPercentRemaining( double cuVoltage )
    {
        const double vMax  = 78;
        const double vMin  = 69;
        const double slope = 100.0 / (vMax-vMin);
        const double b = -vMin * slope;

        int pct = (int)floor(slope * cuVoltage + b);

        // return a number between 0 and 100
        CLIP_TO_LIMITS( 0, pct, 100 );
        return pct;
    }

    void
    convert( const hydrointerfaces::SegwayRmp::Data& internal, orca::PowerData& network )
    {
        network.timeStamp.seconds = internal.seconds;
        network.timeStamp.useconds = internal.useconds;

        // set up data structure for 2 batteries
        network.batteries.resize(2);

        network.batteries[0].name              = "main";
        network.batteries[0].voltage           = internal.mainvolt;
        network.batteries[0].percent           = cuBatteryPercentRemaining( internal.mainvolt );
        network.batteries[0].isBatteryCharging = orca::ChargingUnknown;
        network.batteries[0].secRemaining      = -1;

        network.batteries[1].name              = "ui";
        network.batteries[1].voltage           = internal.uivolt;
        network.batteries[1].percent           = 100;
        network.batteries[1].isBatteryCharging = orca::ChargingUnknown;
        network.batteries[1].secRemaining      = -1;
    }

}

//////////////////////////////////////////////////////////////////////

void
PublisherThread::work()
{
    setMaxHeartbeatInterval( 10 );

    // temp object in hydro format
    hydrointerfaces::SegwayRmp::Data data;

    // temp objects in network format
    orca::Odometry2dData odometry2dData;
    orca::Odometry3dData odometry3dData;
    orca::PowerData      powerData;

    gbxiceutilacfr::Timer odometry2dPublishTimer;
    gbxiceutilacfr::Timer odometry3dPublishTimer;
    gbxiceutilacfr::Timer powerPublishTimer;

    while ( !isStopping() )
    {
        if ( dataStore_.getNext( data, 1000 ) )
        {
            health().ok( "Net loop timed out" );
            continue;
        }

        // Odometry2d
        convert( data, odometry2dData, hardwareInitNeedsReporting2d_ );
        hardwareInitNeedsReporting2d_ = false;
        // check that we were not told to terminate while we were sleeping
        // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
        if ( !isStopping() && odometry2dPublishTimer.elapsed().toSecondsDouble()>=odometry2dPublishInterval_ ) {
            odometry2dI_->localSetAndSend( odometry2dData );
            odometry2dPublishTimer.restart();
        }
        else {
            odometry2dI_->localSet( odometry2dData );
        }

        // Odometry3d
        convert( data, odometry3dData, hardwareInitNeedsReporting3d_ );
        hardwareInitNeedsReporting3d_ = false;
        if ( !isStopping() && odometry3dPublishTimer.elapsed().toSecondsDouble()>=odometry3dPublishInterval_ ) {
            odometry3dI_->localSetAndSend( odometry3dData );
            odometry3dPublishTimer.restart();
        }
        else {
            odometry3dI_->localSet( odometry3dData );
        }

        // Power
        convert( data, powerData );
        if ( !isStopping() && powerPublishTimer.elapsed().toSecondsDouble()>=powerPublishInterval_ ) {
            powerI_->localSetAndSend( powerData );
            powerPublishTimer.restart();
        }
        else {
            powerI_->localSet( powerData );
        }

        // subsystem heartbeat
        health().ok();
    }
}

void
PublisherThread::finalise()
{
    if ( historyStats_.isValid() ) {
        stringstream ss;
        ss << historyStats_.distance()<<" "<<historyStats_.timeInMotion()<<" "<<historyStats_.maxSpeed();
        history_.report( ss.str() );
    }
}

void
PublisherThread::publish( const hydrointerfaces::SegwayRmp::Data &data )
{
    dataStore_.set( data );

    historyStats_.addData( data );

    if ( historyStats_.isValid() ) {
        stringstream ss;
        ss << historyStats_.distance()<<" "<<historyStats_.timeInMotion()<<" "<<historyStats_.maxSpeed();
        history_.report( ss.str() );
    }
}

void
PublisherThread::hardwareInitialised()
{
    hardwareInitNeedsReporting2d_ = true;
    hardwareInitNeedsReporting3d_ = true;

    historyStats_.resetRawData();
}

}

