/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
