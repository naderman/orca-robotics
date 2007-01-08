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
#include <orca/rangescanner2d.h>

namespace localnav {

// fwd decl
class Goal;

//
// @author Alex Brooks
//
// Base class for local navigation drivers.
// The manager sets the goal location (in the robot's coordinate system)
// by modifying the goal.
//
class IDriver
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

    IDriver() {};
    virtual ~IDriver() {};

    //
    // Sets cmd.
    // Returns the state of the algorithm, from the list above.
    //
    virtual DriverState getCommand( bool                               stalled,
                                    const orca::Twist2d               &currentVelocity,
                                    const orca::RangeScanner2dDataPtr  obs,
                                    const Goal                        &goal,
                                    orca::Velocity2dCommand           &cmd ) = 0;

protected: 

};

std::ostream &operator<<( std::ostream &s, IDriver::DriverState state );

}

#endif
