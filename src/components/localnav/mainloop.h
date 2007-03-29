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
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/thread.h>
#include <orcaice/proxy.h>
#include <orcaifaceimpl/consumertypes.h>
#include <orcalocalnav/speedlimiter.h>
#include <orcalocalnav/pathmaintainer.h>
#include <orcalocalnav/clock.h>
#include <localnavutil/idriver.h>
#include <localnavutil/isensormodel.h>
#include <localnavutil/isensordata.h>
#include <localnavutil/icontrol.h>
#include <localnavutil/istate.h>
#include <localnavutil/icontroldata.h>
#include <localnavutil/istatedata.h>

namespace localnav {

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
    void subscribeForLocalisation();
    void subscribeForObservations();
    void subscribeForState();

    void sendCommandToPlatform( const IControlData& cmd );

    // Make sure all our sources of info are OK, and that there's something
    // in all our buffers
    void ensureProxiesNotEmpty();

    /// Set the command to 'stop'
    /// This signature is modified, to allow us to pass in our current state, 
    /// in case that informs us on a better way to issue a "stop" command 
    /// (e.g., knowing our current velocity or steering angle in a bicycle)
    /// We pass the currentState in as a pointer to allow passing NULL.
    void getStopCommand( IControlData* cmd, IStateData* currentState );

    // See if we need to follow a new path, plus
    // see if we should update the world on our progress.
    void checkWithOutsideWorld();

    // Returns true if the timestamps differ by more than a threshold.
    bool areTimestampsDodgy( const ISensorData&                 sensorData,
                             const orca::Localise2dData&        localiseData,
                             const IStateData&                  stateData,
                             double                             threshold );

    // Constrains the max speed
    orcalocalnav::SpeedLimiter *speedLimiter_;

    // Keeps track of the path we're following
    orcalocalnav::PathMaintainer  *pathMaintainer_;

    // Using this driver
    IDriver *driver_;

    // Loaded with this
    DriverFactory &driverFactory_;

    // the type of sensor info we have for planning a path through the world
    ISensorModel* sensorModel_;
    
    // the type of control we will be using
    IControl* controlInterface_;
    // includes state variables not available from odometry
    IState* stateInterface_; 
    
    // Incoming observations and pose info
    // Get observations, pose, and odometric velocity
    orcaifaceimpl::proxiedLocalise2dConsumerPtr locConsumer_;

    orcaice::Proxy<orca::Localise2dData>           *locProxy_;

    orcalocalnav::PathFollower2dI  &pathFollowerInterface_;

    // data types
    ISensorData*                   sensorData_;
    orca::Localise2dData           localiseData_;

    // Outgoing commands: live version
    IControlData*                  cmd_;
    // State data for controller
    IStateData*                    state_;
    // Outgoing commands: test version
    Simulator                     *testSimulator_;

    // A global time reference
    orcalocalnav::Clock &clock_;

    orca::VehicleDescription        vehicleDescr_;

    bool testMode_;

    orcaice::Context context_;
};

}

#endif
