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

#include <Ice/Ice.h> 

#include <iostream>

#include <orcaice/orcaice.h>
#include "skeletongraphicsI.h"

#include <orcapathplan/orcapathplan.h>
#include <orcamisc/orcamisc.h>

#include "astardriver.h"

using namespace std;
using namespace orcaogmap;
using namespace orcapathplan;
using orcaice::operator<<;

namespace pathplanner {

bool AStarDriver::areAllWaypointsInMap(const orca::PathPlanner2dDataPtr & pathDataPtr)
{
    // Check whether startCell is within map
    if ( !ogMap_.coordsWithinMap( startWp_.target.p.x, startWp_.target.p.y) )
    {
        pathDataPtr->result = orca::PathStartNotValid;
        cout << "ERROR(astardriver.cpp): Start waypoint outside map. Returning..." << endl;
        return false;
    }
    cout << "INFO(astardriver.cpp): Start waypoint is within map" << endl;

    // Check whether course path is within map
    for (uint i=0; i<coarsePath_.size(); i++)
    {
        if ( !ogMap_.coordsWithinMap( coarsePath_[i].target.p.x, coarsePath_[i].target.p.y) )
        {
            pathDataPtr->result = orca::PathDestinationNotValid;
            cout << "ERROR(astardriver.cpp): Goal waypoint " << i << " is outside map. Returning..." << endl;
            return false;
        }
    }
    cout << "INFO(astardriver.cpp): Goals path is within map" << endl;

    return true;
}

Cell2D 
AStarDriver::getStartCell()
{
    int cellX, cellY;
    ogMap_.getCellIndices( startWp_.target.p.x, startWp_.target.p.y, cellX, cellY ); 
    return Cell2D( cellX, cellY ); 
}

Cell2D 
AStarDriver::getGoalCell( uint i)
{
    assert( coarsePath_.size() > i );
    int cellX, cellY;
    ogMap_.getCellIndices( coarsePath_[i].target.p.x, coarsePath_[i].target.p.y, cellX, cellY ); 
    return Cell2D( cellX, cellY ); 
}


void AStarDriver::computePath( const orca::OgMapDataPtr          & ogMapDataPtr,
                                       const orca::PathPlanner2dTaskPtr  & taskPtr,
                                       const orca::PathPlanner2dDataPtr  & pathDataPtr )
{
    // Measuring performance
    orcamisc::CpuStopwatch watch;

    // Convert to local storage
    orcaogmap::convert( ogMapDataPtr, ogMap_ );
    startWp_ = taskPtr->start;
    coarsePath_ = taskPtr->coarsePath;

    // check start and goal waypoints
    // error messages and result codes are handled in that function
    if ( !areAllWaypointsInMap(pathDataPtr) ) return;

    // ========== initialize variables ==============
    Cell2D startCell = getStartCell();
    Result result;
    
    // conversion of ogmap to astar format
    int sizeMap = ogMap_.numCellsX() * ogMap_.numCellsY();
    double ogMapDoubles[sizeMap];
    const double offset = 1.0;  // each value from the original ogmap is decreased by one because AStar class assumes value 1 to be unoccupied
    convert( ogMap_, ogMapDoubles, offset );
    
    // instantiate AStar class
    const bool allowDiagonal = true;
    const int obstacleWeight = 255;     // AStar class uses 'obstacleWeight' for occupied and 1 for unoccupied
    assert( ogMap_.metresPerCellX() == ogMap_.metresPerCellY() );
    AStar* aStar = new AStar( ogMapDoubles, ogMap_.numCellsX(), ogMap_.numCellsY(), ogMap_.metresPerCellX(), allowDiagonal, obstacleWeight );     
    // ==============================================
    
    // for each waypoint in the coarse path we need to compute the navigation fct and the path
    for (uint i=0; i<coarsePath_.size(); i++)
    {   
        aStar->m_iStartNode_i = startCell.x();
        aStar->m_iStartNode_j = startCell.y();
        
        Cell2D goalCell = getGoalCell( i );
        aStar->m_iGoalNode_i = goalCell.x();
        aStar->m_iGoalNode_j = goalCell.y();
              
        watch.start();
        if ( !aStar->run() )
        {
            cout << "ERROR(astardriver.cpp): No path found" << endl;
            result = orcapathplan::PathDestinationUnreachable;
            // this will set the result flag accordingly but won't touch the path
            orcapathplan::convert( result, pathDataPtr );
            return;
        }
        watch.stop();
        
        cout << "INFO(astardriver.cpp): Path successfully computed in " << watch.elapsedSeconds() * 1000.0 << " ms." << endl;
        result = orcapathplan::PathOk;
        
        // resulting path
        AStarNodeDeque path;
        
        AStarNode *node = aStar->m_pBestNode;
        while ( node != NULL )
        {
            path.push_front( node );
            node = node->m_pParent;   
        }
        cout << "INFO(astardriver.cpp): Size of path is " << path.size() << endl;   
        
        // ====== Convert to an Orca object in global coordinate system. =====
        // ====== Will append latest path to the total pathDataPtr. ==========
        // ====== Not all data fields are filled in (e.g.tolerances) =========
        orcapathplan::convert( ogMap_, path, result, pathDataPtr );
        // ==================================================================
        
        // set last goal cell as new start cell
        startCell.set( goalCell.x(), goalCell.y() );
    }
    
}

}


