/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_WALKER_RANDOMWALK_ALGORITHM_DRIVER_H
#define ORCA2_WALKER_RANDOMWALK_ALGORITHM_DRIVER_H

#include <hydroutil/timer.h>

#include "algodriver.h"

namespace reactivewalker
{

class RandomDriver : public AlgoDriver
{

public:

    RandomDriver();
    
    // Computes the motion command
    virtual int computeCommand( const orca::RangeScanner2dDataPtr& laserData,
                                const orca::Odometry2dData& odometryData,
                                orca::VelocityControl2dData& commandData );

private:

    hydroutil::Timer randomTimer_;
    hydroutil::Timer avoidTimer_;

    // state machine
    int avoidDirection_;
    bool isAvoiding_;
    bool forceRandomCourseChange_;
    
};

} // namespace

#endif
