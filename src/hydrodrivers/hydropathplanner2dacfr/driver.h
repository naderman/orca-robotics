/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef PATHPLANNER2DACFR_DRIVER_H
#define PATHPLANNER2DACFR_DRIVER_H

#include <hydrointerfaces/pathplanner2d.h>
#include <hydropathplan/ipathplanner2d.h>
#include <memory>

namespace pathplanner2dacfr
{

//
// @brief A wrapper for all the path planners in the hydropathplan library
//
// @author Alex Brooks
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

    std::auto_ptr<hydropathplan::IPathPlanner2d> algorithm_;

    hydroogmap::OgMap ogMap_;

//    hydroutil::Context context_;
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
    hydrointerfaces::PathPlanner2dFactory *createPathPlanner2dDriverFactory();
}

#endif
