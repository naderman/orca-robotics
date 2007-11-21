/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_GENERICDRIVER_H
#define ORCA_PATHPLANNER_GENERICDRIVER_H

#include "algodriver.h"
#include <orcaice/orcaice.h>
#include <orcapathplan/orcapathplan.h>
#include <orca/ogmap.h>
#include <hydroogmap/hydroogmap.h>
#include <vector>

namespace pathplanner {

//
// @author Alex Brooks, Tobias Kaupp
//
class GenericDriver : public AlgoDriver
{

public: 

    GenericDriver( hydropathplan::IPathPlanner2d *pathPlanner,
                   const hydroogmap::OgMap       &ogMap,
                   double                        robotDiameterMetres,
                   double                        traversabilityThreshhold,
                   bool                          doPathOptimization,
                   bool                          jiggleWaypointsOntoClearCells,
                   const orcaice::Context       &context );
    
    ~GenericDriver();
    
    virtual void computePath( const orca::PathPlanner2dTask& task,
                              orca::PathPlanner2dData& pathData );

private: 

    hydropathplan::IPathPlanner2d  *pathPlanner_;
    const hydroogmap::OgMap        &ogMap_;
    hydroogmap::OgMap               grownOgMap_;
    
    double robotDiameterMetres_;
    double traversabilityThreshhold_;
    bool   doPathOptimization_;
    
    void setWaypointParameters( const orca::Waypoint2d *startWp, 
                                const orca::Waypoint2d *goalWp, 
                                const hydropathplan::Cell2DVector &pathSegmentCells,
                                const hydroogmap::OgMap &ogMap,
                                std::vector<orcapathplan::WaypointParameter> &wpParaVector );

    void jiggleOntoClearCells( orca::Path2d &path );

    orca::Path2d jiggledPath_;
    
    bool jiggleWaypointsOntoClearCells_;
    const orcaice::Context context_;
};

}

#endif
