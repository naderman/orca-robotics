/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <orca/velocitycontrol2d.h>
#include <orcaifaceimpl/rangescanner2d.h>
#include <orcaifaceimpl/localise2d.h>
#include <orcaifaceimpl/odometry2d.h>
#include <orcaice/context.h>
#include <orcaice/subsystemthread.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include "speedlimiter.h"
#include "pathmaintainer.h"
#include "clock.h"
#include <orcalocalnav/idriver.h>
#include <hydronavutil/hydronavutil.h>
#include <hydrodll/dynamicload.h>
#include "pathfollowerinterface.h"
#include "testsim/simulator.h"
#include "algorithmevaluator.h"

namespace localnav {

//
// @brief the main executing loop of this component.
//
// Note: this thing self-destructs when run() returns.
//
//
// @author Alex Brooks
//
class MainThread : public orcaice::SubsystemThread
{

public: 

    MainThread( const orcaice::Context &context );

private: 

    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    void getVehicleDescription();
    void getRangeScannerDescription();
    void setup();
    void initPathFollowerInterface();

    void sendCommandToPlatform( const hydronavutil::Velocity &cmd );

    // Make sure all our sources of info are OK, and that there's something
    // in all our buffers
    void ensureProxiesNotEmpty();

    // See if we need to follow a new path, plus
    // see if we should update the world on our progress.
    void checkWithOutsideWorld();

    void getInputs( hydronavutil::Velocity &velocity,
                       hydronavutil::Pose     &localisePose,
                       orca::Time             &poseTime,
                       bool                   &isLocalisationUncertain,
                       std::vector<float>     &obsRanges,
                       orca::Time             &obsTime );

    void stopVehicle();

    // Constrains the max speed
    std::auto_ptr<SpeedLimiter> speedLimiter_;

    // Keeps track of the path we're following
    std::auto_ptr<PathMaintainer>  pathMaintainer_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // The factory which creates the driver
    std::auto_ptr<orcalocalnav::DriverFactory> driverFactory_;
    // Does the work
    std::auto_ptr<orcalocalnav::IDriver> driver_;

    // Incoming observations and pose info
    // Get observations, pose, and odometric velocity
    orcaifaceimpl::StoringRangeScanner2dConsumerImplPtr obsConsumer_;
    orcaifaceimpl::StoringLocalise2dConsumerImplPtr     locConsumer_;
    orcaifaceimpl::StoringOdometry2dConsumerImplPtr     odomConsumer_;

    std::auto_ptr<PathFollowerInterface> pathFollowerInterface_;

    // Outgoing commands: live version
    orca::VelocityControl2dPrx     velControl2dPrx_;
    // Outgoing commands: test version
    Simulator                     *testSimulator_;

    // temporary data
    orca::Localise2dData           orcaLocaliseData_;
    orca::Odometry2dData           orcaOdomData_;
    orca::RangeScanner2dDataPtr    orcaRangeData_;

    // A global time reference
    std::auto_ptr<Clock> clock_;

    orca::VehicleDescription        vehicleDescr_;
    orca::RangeScanner2dDescription scannerDescr_;

    bool testMode_;
    std::auto_ptr<AlgorithmEvaluator> algorithmEvaluator_;

    orcaice::Context context_;
};

}

#endif
