/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANNER_ASTAR_ALGORITHM_DRIVER_H
#define ORCA2_PATHPLANNER_ASTAR_ALGORITHM_DRIVER_H

#include "algodriver.h"

#include <orcapathplan/ipathplanner2d.h>
#include <orcaogmap/orcaogmap.h>


namespace pathplanner
{

class AStarDriver : public AlgoDriver 
{

public:
    AStarDriver( orcaogmap::OgMap &ogMap,
                 double robotDiameterMetres,
                 double traversabilityThreshhold,
                 bool doPathOptimization );
   
    // Computes the path
    virtual void computePath(   const orca::PathPlanner2dTaskPtr  & taskPtr,
                                const orca::PathPlanner2dDataPtr  & pathDataPtr );
private:

//     double robotDiameterMetres_;
//     double traversabilityThreshhold_;
//     bool doPathOptimization_;
    
    orcaogmap::OgMap ogMap_;
    orcapathplan::IPathPlanner2d  *pathPlanner_;
    
//     orca::Path2d coarsePath_;
// 
//     bool areAllWaypointsInMap( const orca::PathPlanner2dDataPtr  & pathDataPtr );
// 
//     // Converts startWp from world to cell coordinate system
//     orcapathplan::Cell2D getStartCell();
// 
//     // Converts goal cell i from world to cell coordinate system
//     orcapathplan::Cell2D getGoalCell( unsigned int i);

};

} // namespace

#endif
