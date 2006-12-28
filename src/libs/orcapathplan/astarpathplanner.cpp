/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include "astarpathplanner.h"
#include "pathplanutils.h"
#include <orcamisc/orcamisc.h>
#include <iostream>

using namespace std;

namespace orcapathplan {

AStarPathPlanner::AStarPathPlanner( const orcaogmap::OgMap ogMap,
                                    double robotDiameterMetres,
                                    double traversabilityThreshhold,
                                    bool   doPathOptimization )
    : ogMap_(ogMap),
      robotDiameterMetres_(robotDiameterMetres),
      traversabilityThreshhold_(traversabilityThreshhold),
      doPathOptimization_(doPathOptimization)
{
    assert( robotDiameterMetres >= 0.0 );
    assert( traversabilityThreshhold >= 0.0 );
    
    // grow map
    orcamisc::CpuStopwatch watch;
    watch.start();
    int robotDiameterCells = robotDiameterInCells( ogMap_, robotDiameterMetres_ );
    orcapathplan::growObstaclesOgMap( ogMap_, traversabilityThreshhold_, robotDiameterCells );
    watch.stop();
    cout << "INFO(astardriver.cpp): growing the map took " << watch.elapsedSeconds() * 1000.0 << " ms " << endl;
    
    // convert to astar map format
    int sizeMap = ogMap_.numCellsX() * ogMap_.numCellsY();
    double ogMapDoubles[sizeMap];
    convert( ogMap_, ogMapDoubles );
        
    // instantiate AStar class
    const bool allowDiagonal = true;
    const double obstacleWeight = 254.0 * traversabilityThreshhold_;
    assert( ogMap_.metresPerCellX() == ogMap_.metresPerCellY() && "AstarDriver currently works for square worlds only" );
    aStar_ = new AStar( ogMapDoubles, ogMap_.numCellsX(), ogMap_.numCellsY(), ogMap_.metresPerCellX(), allowDiagonal, obstacleWeight );     
}

void 
AStarPathPlanner::checkInputs( int          startX,
                                  int           startY,
                                  int           endX,
                                  int           endY ) const
{
    unsigned char val;

    if( ogMap_.tryGridCell( startX, startY, val ) == false )
        throw orcapathplan::Exception( "Start point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::Exception( "Start point was not traversable." );

    if( ogMap_.tryGridCell( endX, endY, val ) == false )
        throw orcapathplan::Exception( "End point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::Exception( "End point was not traversable." );
}

void
AStarPathPlanner::computePath( int          startX,
                                  int           startY,
                                  int           endX,
                                  int           endY,
                                  Cell2DVector &path ) const
{
    cout << "INFO(astarpathplanner.cpp): computePath" << endl;
    
    checkInputs( startX, startY, endX, endY );
    
    Cell2D startCell( startX, startY );
    Cell2D goalCell(  endX,   endY   );
    aStar_->m_iStartNode_i = startCell.x();
    aStar_->m_iStartNode_j = startCell.y();
    aStar_->m_iGoalNode_i = goalCell.x();
    aStar_->m_iGoalNode_j = goalCell.y();
            
    orcamisc::CpuStopwatch watch;
    watch.start();
    if ( !aStar_->run() )
    {
        throw orcapathplan::PathDestinationUnreachabledException( "Path could not be computed" );
    }
    watch.stop();
        
    cout << "INFO(astardriver.cpp): Path successfully computed in " << watch.elapsedSeconds() * 1000.0 << " ms." << endl;

    // ============ resulting path =====================
    AStarNodeDeque nodes;
    AStarNode *node = aStar_->m_pBestNode;
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
        Cell2DVector waycells;      
        watch.start();
        optimizePath( ogMap_, traversabilityThreshhold_, path, waycells );
        watch.stop();
        cout << "INFO(astardriver.cpp): optimizing the path took " << watch.elapsedSeconds() * 1000.0 << " ms " << endl;
        path = waycells;
    }
    // =========================================================
}

}
