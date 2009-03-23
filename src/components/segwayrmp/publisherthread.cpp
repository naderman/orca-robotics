#include "publisherthread.h"
#include <iostream>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

using namespace std;

namespace segwayrmp {

namespace {

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

}

//////////////////////////////////////////////////////////////////////

void
PublisherThread::work()
{
    subStatus().setMaxHeartbeatInterval( 10 );

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
            subStatus().ok( "Net loop timed out" );
            continue;
        }

        // Odometry2d
        convert( data, odometry2dData );
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
        convert( data, odometry3dData );
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
        subStatus().ok();
    }
}

void
PublisherThread::finalise()
{
    stringstream historySS;
    historySS << stats_.distance()<<" "<<stats_.timeInMotion()<<" "<<stats_.maxSpeed();
    history_.setWithFinishSequence( historySS.str() );    
}

}

