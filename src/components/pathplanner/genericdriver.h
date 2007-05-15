/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_GENERICDRIVER_H
#define ORCA_PATHPLANNER_GENERICDRIVER_H

#include "algodriver.h"
#include <orcaice/orcaice.h>
#include <orcapathplan/orcapathplan.h>
#include <orcapathplan/ipathplanner2d.h>
#include <orca/ogmap.h>
#include <orcaogmap/orcaogmap.h>
#include <vector>

namespace pathplanner {

//
// @author Alex Brooks, Tobias Kaupp
//
class GenericDriver : public AlgoDriver
{

public: 

    GenericDriver( orcapathplan::IPathPlanner2d *pathPlanner,
                   const orcaogmap::OgMap       &ogMap,
                   double                        robotDiameterMetres,
                   double                        traversabilityThreshhold,
                   bool                          doPathOptimization,
                   bool                          jiggleWaypointsOntoClearCells,
                   const orcaice::Context       &context );
    
    ~GenericDriver();
    
    virtual void computePath( const orca::PathPlanner2dTask& task,
                              orca::PathPlanner2dData& pathData );

private: 

    orcapathplan::IPathPlanner2d  *pathPlanner_;
    const orcaogmap::OgMap        &ogMap_;
    orcaogmap::OgMap               grownOgMap_;
    
    double robotDiameterMetres_;
    double traversabilityThreshhold_;
    bool   doPathOptimization_;
    
    void setWaypointParameters( const orca::Waypoint2d                       *startWp, 
                                const orca::Waypoint2d                       *goalWp, 
                                int                                           numSegments,
                                std::vector<orcapathplan::WaypointParameter> &wpParaVector );

    void jiggleOntoClearCells( orca::Path2d &path );

    orca::Path2d jiggledPath_;
    
    bool jiggleWaypointsOntoClearCells_;
    const orcaice::Context context_;
};

}

#endif
