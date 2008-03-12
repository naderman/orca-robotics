/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_DRIVER_H
#define ORCA_PATHPLANNER_DRIVER_H

#include <orcaice/orcaice.h>
#include <orca/pathplanner2d.h>
#include <hydroogmap/hydroogmap.h>
#include <hydropathplan/hydropathplan.h>
#include <vector>

namespace pathplanner {

//
// @author Alex Brooks, Tobias Kaupp
//
class Driver
{

public: 

    Driver( hydropathplan::IPathPlanner2d &pathPlanner,
            const hydroogmap::OgMap       &ogMap,
            double                        robotDiameterMetres,
            double                        traversabilityThreshhold,
            bool                          jiggleWaypointsOntoClearCells,
            const orcaice::Context       &context );
    
    void computePath( const orca::PathPlanner2dTask &task,
                      orca::Path2d                  &path );

private: 

    void convertAndAppend( const orca::Waypoint2d                       &startWp, 
                           const orca::Waypoint2d                       &goalWp, 
                           const hydropathplan::Cell2DVector            &pathCells,
                           const hydroogmap::OgMap                      &ogMap,
                           orca::Path2d                                 &path );

    hydropathplan::Cell2DVector computePathSegment( double startX,
                                                    double startY,
                                                    double endX,
                                                    double endY );

    void jiggleOntoClearCells( orca::Path2d &path );

    hydropathplan::IPathPlanner2d  &pathPlanner_;
    const hydroogmap::OgMap        &ogMap_;
    hydroogmap::OgMap               grownOgMap_;
    
    double traversabilityThreshhold_;
    
    orca::Path2d jiggledPath_;
    
    bool jiggleWaypointsOntoClearCells_;
    const orcaice::Context context_;
};

}

#endif
