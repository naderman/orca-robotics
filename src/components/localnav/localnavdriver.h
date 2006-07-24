/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCALNAVDRIVER_H
#define LOCALNAVDRIVER_H

#include <orca/platform2d.h>
#include <orca/rangescanner.h>

namespace localnav {

class LocalNavParameters {
public:

    // Default parameters are nice and slow.  This allows 
    // the robot to hold in place before receiving a goal.
    LocalNavParameters()
        { setToNoGoal(); }

    // The nav params when there's no active goal
    void setToNoGoal()
        {
            maxSpeed    = 0.01;
            maxTurnrate = 0.017; // 1deg
        }

    float maxSpeed;
    float maxTurnrate;
};

std::ostream &operator<<( std::ostream &s, LocalNavParameters &n );

// fwd decl
class GoalWatcher;

//
// @author Alex Brooks
//
// Base class for local navigation drivers.
// The manager sets the goal location (in the robot's coordinate system)
// by modifying the goalWatcher.
//
class LocalNavDriver
{

public: 

    ////////////////////////////////////////////////////////////

    // Constants 
    enum DriverState 
    {
        STATE_GOAL_REACHED,
        STATE_TURNING_AT_GOAL,
        STATE_ESCAPING,
        STATE_MOVING_TO_GOAL
    };

    ////////////////////////////////////////////////////////////

    LocalNavDriver( const GoalWatcher &goalWatcher )
        : goalWatcher_(goalWatcher) {};
    virtual ~LocalNavDriver() {};

    virtual void setLocalNavParameters( LocalNavParameters params )=0;

    //
    // Sets cmd.
    // Returns the state of the algorithm, from the list above.
    //
    virtual DriverState getCommand( bool  stalled,
                                    const orca::Twist2d &currentVelocity,
                                    const orca::RangeScannerDataPtr obs,
                                    orca::Velocity2dCommandPtr &cmd )=0;

protected: 

    // This is how the manager sets the driver's goal.
    const GoalWatcher &goalWatcher_;
};

std::ostream &operator<<( std::ostream &s, LocalNavDriver::DriverState state );

}

#endif
