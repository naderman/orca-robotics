/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <orca/velocitycontrol2d.h>
#include <orca/localise2d.h>
#include <orca/rangescanner2d.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/thread.h>
#include <orcaice/proxy.h>
#include <orcaifaceimpl/ptrproxiedconsumerI.h>
#include <orcaifaceimpl/proxiedconsumerI.h>
#include <orcalocalnav/localnavmanager.h>
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
class MainLoop : public orcaice::Thread
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

    // The class that does the navigating
    orcalocalnav::LocalNavManager *localNavManager_;

    // Keeps track of the path we're following
    orcalocalnav::PathMaintainer  *pathMaintainer_;

    // Using this driver
    IDriver *driver_;

    // Loaded with this
    DriverFactory &driverFactory_;

    // Incoming observations and pose info
    // Get observations, pose, and odometric velocity
    orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr> *obsConsumer_;
    orcaifaceimpl::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>     *locConsumer_;
    orcaifaceimpl::ProxiedConsumerI<orca::Odometry2dConsumer,orca::Odometry2dData>     *odomConsumer_;

    orcaice::PtrProxy<orca::RangeScanner2dDataPtr> *obsProxy_;
    orcaice::Proxy<orca::Localise2dData>           *locProxy_;
    orcaice::Proxy<orca::Odometry2dData>           *odomProxy_;

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

    // consumers
    orca::Odometry2dConsumerPrx     odomConsumerPrx_;
    orca::Localise2dConsumerPrx     locConsumerPrx_;
    orca::RangeScanner2dConsumerPrx obsConsumerPrx_;

    orcaice::Context context_;
};

}

#endif
