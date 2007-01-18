/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

#include <localnavmanager.h>

namespace localnav {

class PathMaintainer;
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
    MainLoop( LocalNavManager                                &localNavManager,
              orcaice::PtrProxy<orca::RangeScanner2dDataPtr> &obsProxy,
              orcaice::Proxy<orca::Localise2dData>           &locProxy,
              orcaice::Proxy<orca::Odometry2dData>           &odomProxy,
              PathFollower2dI                                &pathFollowerInterface,
              orca::VelocityControl2dPrx                     &velControl2dPrx,
              PathMaintainer                                 &pathMaintainer,
              orca::PathFollower2dConsumerPrx                &pathPublisher,
              const orcaice::Context                         &context );

    // This version is for simulator-based testing.
    MainLoop( LocalNavManager                                &localNavManager,
              orcaice::PtrProxy<orca::RangeScanner2dDataPtr> &obsProxy,
              orcaice::Proxy<orca::Localise2dData>           &locProxy,
              orcaice::Proxy<orca::Odometry2dData>           &odomProxy,
              PathFollower2dI                                &pathFollowerInterface,
              Simulator                                      &testSimulator,
              PathMaintainer                                 &pathMaintainer,
              orca::PathFollower2dConsumerPrx                &pathPublisher,
              const orcaice::Context                         &context );

    ~MainLoop();

    virtual void run();

private: 

    void initInterfaces();

    // Make sure all our sources of info are OK, and that there's something
    // in all our buffers
    void ensureProxiesNotEmpty();

    // Set the command to 'stop'
    void getStopCommand( orca::VelocityControl2dData& cmd );

    // See if we need to follow a new path, plus
    // see if we should update the world on our progress.
    void checkWithOutsideWorld( PathMaintainer& pathMaintainer );

    // Returns true if the timestamps differ by more than a threshold.
    bool areTimestampsDodgy( const orca::RangeScanner2dDataPtr &rangeData,
                             const orca::Localise2dData&        localiseData,
                             const orca::Odometry2dData&        odomData,
                             double                             threshold );

    // The class that does the navigating
    LocalNavManager &localNavManager_;

    // Incoming observations and pose info
    orcaice::PtrProxy<orca::RangeScanner2dDataPtr> &obsProxy_;
    orcaice::Proxy<orca::Localise2dData>   &locProxy_;
    orcaice::Proxy<orca::Odometry2dData>   &odomProxy_;

    PathFollower2dI                  &pathFollowerInterface_;

    // data types
    orca::Localise2dData           localiseData_;
    orca::Odometry2dData           odomData_;
    orca::RangeScanner2dDataPtr    rangeData_;
    orca::VelocityControl2dData    velocityCmd_;

    // Outgoing commands: live version
    orca::VelocityControl2dPrx     velControl2dPrx_;
    // Outgoing commands: test version
    Simulator                     *testSimulator_;

    bool testMode_;

    // Keeps track of the path we're following
    PathMaintainer  &pathMaintainer_;

    // The object to publish path updates to
    orca::PathFollower2dConsumerPrx              &pathPublisher_;

    orcaice::Context context_;
};

}

#endif
