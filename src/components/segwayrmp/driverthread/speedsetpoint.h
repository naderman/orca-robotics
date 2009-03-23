/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SEGWAYRMP_SPEEDSETPOINT_H
#define SEGWAYRMP_SPEEDSETPOINT_H

#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace segwayrmpdriverthread {

//
// @brief Represents a speed set-point, for managing acceleration limits
//
// @author Alex Brooks
//
class SpeedSetPoint
{

public: 

    SpeedSetPoint( double maxForwardAcceleration, 
                   double maxReverseAcceleration );

    // This needs to be called every time around the driving loop,
    // So we know the call frequency.
    void evaluateDt();

    // Set the set-point
    void set( double speed );

    // Returns: the speed we should send to the robot right now.
    // Sets setPointReached to true if future calls (in the absence of a new set() call) will
    //   return the same currentSpeed.
    double currentCmdSpeed( bool &setPointReached );

private: 

    double dt_;

    double maxForwardAcceleration_;
    double maxReverseAcceleration_;

    double setPoint_;
    // Store the prev one just for a safety check.
    double prevSetPoint_;
    double currentCmdSpeed_;

    gbxiceutilacfr::Timer timer_;
};

}

#endif
