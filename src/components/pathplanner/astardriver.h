/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANNER_ASTAR_ALGORITHM_DRIVER_H
#define ORCA2_PATHPLANNER_ASTAR_ALGORITHM_DRIVER_H

#include "algodriver.h"
#include "genericdriver.h"

#include <orcapathplan/ipathplanner2d.h>
#include <orcaogmap/orcaogmap.h>


namespace pathplanner
{
    
//
// @author Tobias Kaupp
//
class AStarDriver : public AlgoDriver 
{

public:
    AStarDriver( const orcaogmap::OgMap& ogMap,
                 double robotDiameterMetres,
                 double traversabilityThreshhold,
                 bool doPathOptimization );
    
   ~AStarDriver();
    
    // Computes the path
    virtual void computePath( const orca::PathPlanner2dTask& task,
                              orca::PathPlanner2dData& pathData );
private:
    orcapathplan::IPathPlanner2d *pathPlanner_;
    GenericDriver                *genericDriver_;
    

};

} // namespace

#endif
