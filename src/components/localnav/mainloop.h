/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <orca/velocitycontrol2d.h>
#include <orca/localise2d.h>
#include <orca/rangescanner2d.h>
#include <orcaice/context.h>
#include <hydroutil/thread.h>
#include <hydroutil/proxy.h>
#include <orcaifaceimpl/proxiedconsumers.h>
#include <orcalocalnav/speedlimiter.h>
#include <orcalocalnav/pathmaintainer.h>
#include <orcalocalnav/clock.h>
#include <localnavutil/idriver.h>

namespace localnav {

// class PathMaintainer;
class PathFollower2dI;
class Simulator;

//
// @brief the main executing loop of this component.
//
// Note: this thing self-destructs when run() returns.
//
//
// @author Alex Brooks
//
class MainLoop : public hydroutil::Thread
{

public: 

    // This version interacts with the real world
    MainLoop( DriverFactory                   &driverFactory,
              orcalocalnav::Clock             &clock,
              orcalocalnav::PathFollower2dI   &pathFollowerInterface,
              const orcaice::Context          &context );

    // This version is for simulator-based testing.
    MainLoop( DriverFactory                   &driverFactory,
              orcalocalnav::Clock             &clock,
              orcalocalnav::PathFollower2dI   &pathFollowerInterface,
              Simulator                       &testSimulator,
              const orcaice::Context          &context );

    ~MainLoop();

    virtual void run();

private: 

    void setup();
    void initInterfaces();
    void connectToController();
    void subscribeForOdometry();
    void subscribeForLocalisation();
    void subscribeForObservations();

    void sendCommandToPlatform( const orca::VelocityControl2dData& cmd );

    // Make sure all our sources of info are OK, and that there's something
    // in all our buffers
    void ensureProxiesNotEmpty();

    // Set the command to 'stop'
    void getStopCommand( orca::VelocityControl2dData& cmd );

    // See if we need to follow a new path, plus
    // see if we should update the world on our progress.
    void checkWithOutsideWorld();

    // Returns true if the timestamps differ by more than a threshold.
    bool areTimestampsDodgy( const orca::RangeScanner2dDataPtr &rangeData,
                             const orca::Localise2dData&        localiseData,
                             const orca::Odometry2dData&        odomData,
                             double                             threshold );

    // Constrains the max speed
    orcalocalnav::SpeedLimiter *speedLimiter_;

    // Keeps track of the path we're following
    orcalocalnav::PathMaintainer  *pathMaintainer_;

    // Using this driver
    IDriver *driver_;

    // Loaded with this
    DriverFactory &driverFactory_;

    // Incoming observations and pose info
    // Get observations, pose, and odometric velocity
    orcaifaceimpl::ProxiedRangeScanner2dConsumerImplPtr obsConsumer_;
    orcaifaceimpl::ProxiedLocalise2dConsumerImplPtr     locConsumer_;
    orcaifaceimpl::ProxiedOdometry2dConsumerImplPtr     odomConsumer_;

    hydroutil::Proxy<orca::RangeScanner2dDataPtr> *obsProxy_;
    hydroutil::Proxy<orca::Localise2dData>        *locProxy_;
    hydroutil::Proxy<orca::Odometry2dData>        *odomProxy_;

    orcalocalnav::PathFollower2dI  &pathFollowerInterface_;

    // data types
    orca::Localise2dData           localiseData_;
    orca::Odometry2dData           odomData_;
    orca::RangeScanner2dDataPtr    rangeData_;

    // Outgoing commands: live version
    orca::VelocityControl2dPrx     velControl2dPrx_;
    // Outgoing commands: test version
    Simulator                     *testSimulator_;

    // A global time reference
    orcalocalnav::Clock &clock_;

    orca::VehicleDescription        vehicleDescr_;
    orca::RangeScanner2dDescription scannerDescr_;

    bool testMode_;

    orcaice::Context context_;
};

}

#endif
