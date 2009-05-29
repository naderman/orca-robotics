/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_DRIVER_H
#define ORCA_PATHPLANNER_DRIVER_H

#include <orcaice/context.h>
#include <orca/pathplanner2d.h>
#include <hydropathplan/hydropathplan.h>
#include <hydrointerfaces/pathplanner2d.h>
#include <vector>

namespace pathplanner {

//
// @author Alex Brooks, Tobias Kaupp
//
class Driver
{

public: 

    Driver( hydrointerfaces::PathPlanner2d &hydroDriver,
            double                          intermediateWaypointDistTolerance,
            const orcaice::Context         &context );
    
    void computePath( const orca::PathPlanner2dTask &task,
                      orca::Path2d                  &path );

private: 

    std::vector<hydrointerfaces::PathPlanner2d::Point>
    computePathSegment( double startX,
                        double startY,
                        double endX,
                        double endY );

    hydrointerfaces::PathPlanner2d &hydroDriver_;
    
    double intermediateWaypointDistTolerance_; 
    
    const orcaice::Context context_;
};

}

#endif
