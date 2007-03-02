/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCALNAVMANAGER_H
#define LOCALNAVMANAGER_H

#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orca/localise2d.h>
#include <orca/velocitycontrol2d.h>
#include <orca/rangescanner2d.h>
#include <orca/pathfollower2d.h>
#include <orcanavutil/orcanavutil.h>
#include <localnavutil/idriver.h>
#include <orcaice/heartbeater.h>

namespace localnav {

class LocalNavParameters;
class Goal;
// class PathMaintainer;

//
// Takes goals and localisation info in the global reference frame.
// Feeds information to the driver, all in the robot's local frame
// of reference.
//
// @author Alex Brooks
//
class LocalNavManager
{

public: 

    LocalNavManager( IDriver                &driver,
                     const orcaice::Context &context );

    // Driver sets everything up, and is told what time it is
    void init( const orca::Time &time )
        { driver_.init( time ); }

    // Call this when you finish the path.
    void reset()
        { driver_.reset(); }

    // Check if there is a goal coming up.
    // If there is no goal, stop the robot and reset the pathplanning driver
    bool checkNextGoal( const orca::Localise2dData&       localiseData,
                        std::vector<Goal>&                currentGoals,
                        bool&                             uncertainLocalisation,
                        orca::VelocityControl2dData&      cmd );

private: 

    void maybeSendHeartbeat( bool haveGoal );
    
    // The driver itself
    IDriver   &driver_;

    // Maintain these for heartbeat messages
    double            secondsBehindSchedule_;

    orcaice::Heartbeater heartbeater_;
    orcaice::Context  context_;
};

}

#endif
