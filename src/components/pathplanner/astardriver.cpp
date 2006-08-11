/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <Ice/Ice.h> 

#include <iostream>

#include <orcaice/orcaice.h>

#include <orcapathplan/orcapathplan.h>
#include <orcamisc/orcamisc.h>

#include "astardriver.h"

using namespace std;
using namespace orcaogmap;
using namespace orcapathplan;

namespace pathplanner {

bool AStarDriver::areAllWaypointsInMap(const orca::PathPlanner2dDataPtr & pathDataPtr)
{
    // Check whether startCell is within map
    if ( !ogMap_.coordsWithinMap( coarsePath_[0].target.p.x, coarsePath_[0].target.p.y) )
    {
        pathDataPtr->result = orca::PathStartNotValid;
        cout << "ERROR(astardriver.cpp): Start waypoint outside map. Returning..." << endl;
        return false;
    }
    cout << "INFO(astardriver.cpp): Start waypoint is within map" << endl;

    // Check whether course path is within map
    for (unsigned int i=0; i<coarsePath_.size(); i++)
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
    ogMap_.getCellIndices( coarsePath_[0].target.p.x, coarsePath_[0].target.p.y, cellX, cellY ); 
    return Cell2D( cellX, cellY ); 
}

Cell2D 
AStarDriver::getGoalCell( unsigned int i)
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
    coarsePath_ = taskPtr->coarsePath;

    // check start and goal waypoints
    // error messages and result codes are handled in that function
    if ( !areAllWaypointsInMap(pathDataPtr) ) return;

    // ========== initialize variables ==============
    Cell2D startCell = getStartCell();
    Result result;
    
    // conversion of ogmap to AStar format
    watch.start();
    int robotDiameterCells = robotDiameterInCells( ogMap_, robotDiameterMetres_ );
    growObstaclesOgMap( ogMap_, traversabilityThreshhold_, robotDiameterCells );
    watch.stop();
    cout << "INFO(astardriver.cpp): growing the map took " << watch.elapsedSeconds() * 1000.0 << " ms " << endl;
    int sizeMap = ogMap_.numCellsX() * ogMap_.numCellsY();
    double ogMapDoubles[sizeMap];
    convert( ogMap_, ogMapDoubles );
    
    // instantiate AStar class
    const bool allowDiagonal = true;
    const double obstacleWeight = 254.0 * traversabilityThreshhold_;
    assert( ogMap_.metresPerCellX() == ogMap_.metresPerCellY() );
    AStar* aStar = new AStar( ogMapDoubles, ogMap_.numCellsX(), ogMap_.numCellsY(), ogMap_.metresPerCellX(), allowDiagonal, obstacleWeight );     
    // ==============================================
    
    // for each waypoint in the coarse path we need to compute the navigation fct and the path
    for (unsigned int i=1; i<coarsePath_.size(); i++)
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
        
        // ============ resulting path =====================
        Cell2DVector path;
        AStarNodeDeque nodes;
        AStarNode *node = aStar->m_pBestNode;
        while ( node != NULL )
        {
            nodes.push_front( node );
            node = node->m_pParent;   
        }
        cout << "INFO(astardriver.cpp): Size of path is " << path.size() << endl;  
        
        for (unsigned int k=0; k<nodes.size(); k++)
        {
            Cell2D cell(nodes[k]->cell_i, nodes[k]->cell_j);
            path.push_back( cell );
        }
        // ===================================================
        
        // ============= Optional path optimization ================
        if ( doPathOptimization_ )
        {
            // separate full path into a optimized short path
            Cell2DVector waycells;      
            watch.start();
            optimizePath( ogMap_, path, waycells, robotDiameterMetres_ );
            watch.stop();
            cout << "INFO(astardriver.cpp): optimizing the path took " << watch.elapsedSeconds() * 1000.0 << " ms " << endl;
            path = waycells;
        }
        // =========================================================
        
        // ====== Convert to an Orca object in global coordinate system. =====
        // ====== Will append latest path to the total pathDataPtr. ==========
        // ====== Not all data fields are filled in (e.g.tolerances) =========
        if (i==0)
        {
            // the first time we'll have to insert the start cell
            path.insert(path.begin(),startCell);
        }
        orcapathplan::convert( ogMap_, path, result, pathDataPtr );
        // ==================================================================
        
        // set last goal cell as new start cell
        startCell.set( goalCell.x(), goalCell.y() );
    }
    
}

}


