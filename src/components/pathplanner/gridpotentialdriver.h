/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANNER_GRIDPOTENTIAL_ALGORITHM_DRIVER_H
#define ORCA2_PATHPLANNER_GRIDPOTENTIAL_ALGORITHM_DRIVER_H

#include "algodriver.h"

// only for Waypoint2d and Path2d data structures
#include <orca/pathfollower2d.h>

#include <orcaogmap/orcaogmap.h>


namespace pathplanner
{

class GridPotentialDriver : public AlgoDriver 
{

public:
    GridPotentialDriver( const orcapathplan::Config & config, SkeletonGraphicsI* skelGraphicsI, bool useSkeleton )
        : AlgoDriver( config, skelGraphicsI, useSkeleton ) {};
    
    // Computes the path
    virtual void computePath(   const orca::OgMapDataPtr          & ogMapDataPtr,
                                const orca::PathPlanner2dTaskPtr  & taskPtr,
                                const orca::PathPlanner2dDataPtr  & pathDataPtr );
private:
    orcaogmap::OgMap ogMap_;
    orca::Waypoint2d startWp_;
    orca::Path2d coarsePath_;

    bool areAllWaypointsInMap( const orca::PathPlanner2dDataPtr  & pathDataPtr );

    // Converts startWp from world to cell coordinate system
    orcapathplan::Cell2D getStartCell();

    // Converts goal cell i from world to cell coordinate system
    orcapathplan::Cell2D getGoalCell( unsigned int i);

    // Local member function which calls all the library functions used to compute the navigation function using the skeleton method
    bool calcSkeletonNavigationLocal( orcapathplan::FloatMap & navMap, orcapathplan::FloatMap & distGrid,  orcapathplan::Cell2DVector skel, orcapathplan::Cell2D & startCell );

    // Displays the skeleton in the GUI
    void displaySkeleton( orcapathplan::Cell2DVector & skel );
};

} // namespace

#endif
