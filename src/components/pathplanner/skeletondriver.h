/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_SKELETONDRIVER_H
#define ORCA_PATHPLANNER_SKELETONDRIVER_H

#include "algodriver.h"
#include <orcapathplan/orcapathplan.h>
#include <orcapathplan/ipathplanner2d.h>
#include <orca/ogmap.h>
#include <orcaogmap/orcaogmap.h>
#include <vector>


namespace pathplanner {

class SkeletonGraphicsI;
//
// The 'skeletonnav' driver.
//
// @author Alex Brooks, Tobias Kaupp
//
class SkeletonDriver : public AlgoDriver
{

public: 

    SkeletonDriver( orca::OgMapDataPtr &ogMapDataPtr,
                    double robotDiameterMetres,
                    double traversabilityThreshhold,
                    bool   doPathOptimization,
                    bool   useSparseSkeleton );
    ~SkeletonDriver();
    
    virtual void computePath( const orca::PathPlanner2dTaskPtr &taskPtr,
                              const orca::PathPlanner2dDataPtr &pathDataPtr );
    
    void setGraphics( SkeletonGraphicsI* skelGraphicsI );

private: 

    SkeletonGraphicsI             *skelGraphicsI_;
    orcaogmap::OgMap               ogMap_;
    orcapathplan::IPathPlanner2d  *pathPlanner_;
    
    double robotDiameterMetres_;
    double traversabilityThreshhold_;
    bool   doPathOptimization_;
    bool   useSparseSkeleton_; 
    
    void addWaypointParameters( std::vector<orcapathplan::WaypointParameter> &wpParaVector, 
                                orca::Waypoint2d                             *startWp, 
                                orca::Waypoint2d                             *goalWp, 
                                int                                           numSegments );
};

}

#endif
