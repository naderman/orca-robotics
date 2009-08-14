/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef PATHPLANNER2DFAKE_DRIVER_H
#define PATHPLANNER2DFAKE_DRIVER_H

#include <hydrointerfaces/pathplanner2d.h>
#include <hydropathplan/ipathplanner2d.h>
#include <memory>

namespace pathplanner2dfake
{

//
// @brief A trivial interpolator between start and goal.
//
class Driver : public hydrointerfaces::PathPlanner2d
{

public: 

    Driver( const hydrointerfaces::PathPlanner2d::Config &config,
            const hydroutil::Context                     &context );

    std::vector<hydrointerfaces::PathPlanner2d::Point>
    computePath( const Point &start,
                 const Point &goal );

private: 

    // number of extra points this "path planner" will insert between the start and the goal points.
    // The points are inserted along a straight line.
    // Default value is 0.
    int numInsertedPoints_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::PathPlanner2dFactory
{
public:
    hydrointerfaces::PathPlanner2d*
    createDriver( const hydrointerfaces::PathPlanner2d::Config &config,
                  const hydroutil::Context                     &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::PathPlanner2dFactory *createDriverFactory();
}

#endif
