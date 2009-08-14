/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "sparseskeletonpathplanner.h"
#include <hydropathplan/sparseskel/sparseskel.h>
#include <hydropathplan/sparseskel/solve.h>
#include <hydropathplan/latombeutil.h>
#include <hydropathplan/jigglewaypoints.h>
#include <hydroutil/cpustopwatch.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace hydropathplan::sparseskel;

namespace hydropathplan {

SparseSkeletonPathPlanner::SparseSkeletonPathPlanner( const hydroogmap::OgMap &ogMap,
                                                      double robotDiameterMetres,
                                                      double traversabilityThreshhold,
                                                      bool   doPathOptimization,
                                                      bool   addExtraNodes,
                                                      double extraNodeResolution, 
                                                      bool   jiggleBadWaypoints,
                                                      const  CostEvaluator &costEvaluator )
    : ogMap_(ogMap),
      robotDiameterMetres_(robotDiameterMetres),
      traversabilityThreshhold_(traversabilityThreshhold),
      doPathOptimization_(doPathOptimization),
      jiggleBadWaypoints_(jiggleBadWaypoints)
{
    assert( traversabilityThreshhold >= 0.0 && traversabilityThreshhold <= 1.0 );

    // Measuring performance
    hydroutil::CpuStopwatch watch;

    watch.start();

    //
    // Build the dense skeleton
    //
    FloatMap distGrid;
    bool success = latombeutil::computeSkeleton( ogMap_,
                                                 skel_,
                                                 distGrid,
                                                 traversabilityThreshhold,
                                                 robotDiameterMetres );

    latombeutil::computeCostsFromDistGrid( distGrid,
                                           costMap_,
                                           ogMap_.metresPerCell(),
                                           costEvaluator );

    watch.stop();
    cout << "TRACE(sparseskeletonpathplanner.cpp): computeSkeleton took " << watch.elapsedSeconds() << "s" << endl;
    
    if ( !success )
    {
        throw hydropathplan::Exception( ERROR_INFO, "Failed to compute skeleton over OG Map" );
    }

    watch.start();

    //
    // convert to sparse skeleton
    //

    // grow obstacles (wait till now because dense skeleton stuff takes robot size into account)

    grownOgMap_ = ogMap_;

    // Maybe a bit of a hack...
    // Grow by half the robot diameter - 1, to avoid the possibility of growing over the dense skeleton.
    int robotDiameterCells = util::robotDiameterInCells( ogMap_, robotDiameterMetres );
    util::growObstaclesOgMap( grownOgMap_, traversabilityThreshhold, robotDiameterCells/2-1 );

    planOgMap_ = &(grownOgMap_);

    // cout<<"TRACE(sparseskeletonpathplanner.cpp): planOgMap_: " << endl << hydroogmap::toText(*planOgMap_) << endl;

    sparseSkel_ = new sparseskel::SparseSkel( (*planOgMap_),
                                              traversabilityThreshhold_,
                                              skel_,
                                              costMap_,
                                              addExtraNodes,
                                              extraNodeResolution );
}

SparseSkeletonPathPlanner::~SparseSkeletonPathPlanner()
{
    delete sparseSkel_;
}

void 
SparseSkeletonPathPlanner::checkAndFixInputs( int &startX,
                                              int &startY,
                                              int &endX,
                                              int &endY,
                                              bool jiggleBadWaypoints ) const
{
    unsigned char val;

    try {
        if( ogMap_.tryGridCell( startX, startY, val ) == false )
            throw hydropathplan::PathStartNotValidException( ERROR_INFO, "Start point was not within the map." );
        if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
            throw hydropathplan::PathStartNotValidException( ERROR_INFO, "Start point was not traversable." );
        if ( grownOgMap_.gridCell(startX,startY) > (unsigned char) ((traversabilityThreshhold_)*254.0) )
            throw hydropathplan::PathStartNotValidException( ERROR_INFO, "Robot can't fit on start point (too close to obstacle)." );
    }
    catch ( std::exception &e )
    {
        if ( !jiggleBadWaypoints_ ) throw;
        try {
            jiggleOntoClearCell( startX, startY, ogMap_, traversabilityThreshhold_ );
        }
        catch ( hydropathplan::Exception &jiggleE )
        {
            throw hydropathplan::PathStartNotValidException( ERROR_INFO, e.what() + string("  ") + jiggleE.what() );
        }
    }

    try {
        if( ogMap_.tryGridCell( endX, endY, val ) == false )
            throw hydropathplan::PathDestinationNotValidException( ERROR_INFO, "End point was not within the map." );
        if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
            throw hydropathplan::PathDestinationNotValidException( ERROR_INFO, "End point was not traversable." );

        if ( grownOgMap_.gridCell(endX,endY) > (unsigned char) ((traversabilityThreshhold_)*254.0) )
            throw hydropathplan::PathDestinationNotValidException( ERROR_INFO, "Robot can't fit on end point (too close to obstacle)." );
    }
    catch ( std::exception &e )
    {
        if ( !jiggleBadWaypoints_ ) throw;
        try {
            jiggleOntoClearCell( endX, endY, ogMap_, traversabilityThreshhold_ );
        }
        catch ( hydropathplan::Exception &jiggleE )
        {
            throw hydropathplan::PathDestinationNotValidException( ERROR_INFO, e.what() + string("  ") + jiggleE.what() );
        }
    }
}

bool isIncluded( const std::vector<SparseSkelNode*> &v, SparseSkelNode *n ) 
{
    for ( unsigned int i=0; i < v.size(); i++ )
    {
        if ( v[i] == n )
            return true;
    }
    return false;    
}

void
SparseSkeletonPathPlanner::findClosestNode( const Cell2D                        &cell,
                                            ContiguousSparseSkel               *&cSkel,
                                            SparseSkelNode                     *&closestNode,
                                            const std::vector<SparseSkelNode*>  &nodesToExclude ) const
{
    assert( (int)(nodesToExclude.size()) < sparseSkel_->numNodes() );

    float minDist = 1e9;
    closestNode = NULL;
    cSkel       = NULL;
    for ( unsigned int i=0; i < sparseSkel_->contiguousSkels().size(); i++ )
    {
        const std::vector<SparseSkelNode*> &nodes = sparseSkel_->contiguousSkels()[i]->nodes();
        for ( unsigned int j=0; j < nodes.size(); j++ )
        {
            float thisDist = hypot( nodes[j]->pos.x()-cell.x(),
                                    nodes[j]->pos.y()-cell.y() );
            if ( thisDist < minDist && !isIncluded( nodesToExclude, nodes[j] ) )
            {
                minDist     = thisDist;
                cSkel       = sparseSkel_->contiguousSkels()[i];
                closestNode = nodes[j];
            }
        }
    }
    assert( closestNode != NULL );
    assert( cSkel != NULL );
}
                                        

void 
SparseSkeletonPathPlanner::connectCell2SparseSkel( const Cell2D          &cellOnDenseSkel,
                                                   ContiguousSparseSkel *&contiguousSparseSkel,
                                                   SparseSkelNode       *&node ) const
{
    // cout<<"TRACE(sparseskeletonpathplanner.cpp): connectCell2SparseSkel(): " << cellOnDenseSkel << endl;

    std::vector<SparseSkelNode*> consideredNodes;

    while ( (int) (consideredNodes.size()) < sparseSkel_->numNodes() )
    {

        //
        // try to hook up to the nodes in order of increasing distance.
        //
        findClosestNode( cellOnDenseSkel,
                         contiguousSparseSkel,
                         node,
                         consideredNodes );
        assert( node != NULL );
        assert( contiguousSparseSkel != NULL );

        if ( util::losExists( (*planOgMap_), traversabilityThreshhold_, cellOnDenseSkel, node->pos ) )
        {
            // found it!
            return;
        }
        else
        {
            consideredNodes.push_back( node );
        }
    }
    std::stringstream ss;
    ss << "Couldn't connect cell at " << cellOnDenseSkel << " to sparse skel";
    throw hydropathplan::Exception( ERROR_INFO, ss.str() );
}

void
SparseSkeletonPathPlanner::planAlongSparseSkel( const ContiguousSparseSkel   &contiguousSparseSkel,
                                                const SparseSkelNode         &startNode, 
                                                const SparseSkelNode         &goalNode,
                                                std::vector<const SparseSkelNode*> &nodePath ) const
{
    // cout<<"----------- planAlongSparseSkel() -------------" << endl;
    hydropathplan::sparseskel::findShortestPath( contiguousSparseSkel,
                                                 &startNode,
                                                 &goalNode,
                                                 nodePath );
}

void
SparseSkeletonPathPlanner::convertToCellVector( const std::vector<Cell2D>          &startCells,
                                                const std::vector<Cell2D>          &goalCells,
                                                std::vector<const SparseSkelNode*> &nodePath,
                                                Cell2DVector                       &path ) const
{
    // cout<<"----------- convertToCellVector() -----------" << endl;

    path.resize(0);

    for ( unsigned int i=0; i < startCells.size(); i++ )
        path.push_back( startCells[0] );

    // Do we need the first node?
    int firstNode = 0;
    if ( nodePath.size() > 1 )
    {
        float distFull = hydropathplan::sparseskel::distance( startCells.back(), nodePath[0]->pos ) + 
                         hydropathplan::sparseskel::distance( nodePath[0]->pos, nodePath[1]->pos );
        float distCut  = hydropathplan::sparseskel::distance( startCells.back(), nodePath[1]->pos );
        if ( distCut < distFull &&
             util::losExists( (*planOgMap_), traversabilityThreshhold_, startCells.back(), nodePath[1]->pos ) )
            firstNode = 1;
    }

    // Do we need the last node?
    int lastNode = nodePath.size()-1;
    if ( nodePath.size() - firstNode > 1 )
    {
        float distFull = hydropathplan::sparseskel::distance( nodePath[nodePath.size()-2]->pos, nodePath[nodePath.size()-1]->pos ) + 
                         hydropathplan::sparseskel::distance( nodePath[nodePath.size()-1]->pos, goalCells[0] );
        float distCut  = hydropathplan::sparseskel::distance( nodePath[nodePath.size()-2]->pos, goalCells[0] );
        if ( distCut < distFull &&
             util::losExists( (*planOgMap_), traversabilityThreshhold_, nodePath[nodePath.size()-2]->pos, goalCells[0] ) )
                lastNode = nodePath.size()-2;
    }

    // Is it even worth going via the skeleton?
    if ( lastNode-firstNode == 0 &&
         util::losExists( (*planOgMap_), traversabilityThreshhold_, startCells.back(), goalCells.front() ) )
    {
        // don't bother with the skeleton
    }
    else
    {
        for ( int i=firstNode; i <= lastNode; i++ )
        {
            path.push_back( nodePath[i]->pos );
        }
    }
    for ( unsigned int i=0; i < goalCells.size(); i++ )
    {
        path.push_back( goalCells[i] );
    }

#ifndef NDEBUG
    // Double-check that the path is OK
    for ( unsigned int i=0; i < path.size()-1; i++ )
    {
        assert( util::losExists( *planOgMap_, traversabilityThreshhold_, path[i], path[i+1] ) );
    }
#endif

//     cout<<"TRACE(sparseskeletonpathplanner.cpp): full path is: " << endl;
//     for ( unsigned int i=0; i < path.size(); i++ )
//     {
//         cout << "  " << path[i] << endl;
//     }
}

void
SparseSkeletonPathPlanner::optimisePath( Cell2DVector &path ) const
{
    Cell2DVector optimisedPath;
    util::optimizePath( grownOgMap_, traversabilityThreshhold_, path, optimisedPath );
    path = optimisedPath;
}

void 
SparseSkeletonPathPlanner::computePath( int           startX,
                                        int           startY,
                                        int           endX,
                                        int           endY,
                                        Cell2DVector &path ) const
{
    // cout << "=============== computePath =================" << endl;

    // First connect the endpoints to the dense skel
    int fixedStartX = startX, fixedStartY = startY;
    int fixedEndX = endX, fixedEndY = endY;
    checkAndFixInputs( fixedStartX, fixedStartY, fixedEndX, fixedEndY, jiggleBadWaypoints_ );
    Cell2D startCell( fixedStartX, fixedStartY );
    Cell2D goalCell(  fixedEndX,   fixedEndY   );
    
    hydroutil::CpuStopwatch watch;
    watch.start();

    //
    // 2. Connect cells to the sparse skeleton,
    //    by finding the closest sparse node.
    //
    ContiguousSparseSkel *startContiguousSparseSkel;
    ContiguousSparseSkel *goalContiguousSparseSkel;
    SparseSkelNode *startNode;
    SparseSkelNode *goalNode;

    try {
        connectCell2SparseSkel( startCell,
                                startContiguousSparseSkel,
                                startNode );

        connectCell2SparseSkel( goalCell,
                                goalContiguousSparseSkel,
                                goalNode );
    }
    catch ( hydropathplan::Exception &e )
    {
        cout << "ERROR(sparseskeletonpathplanner.cpp): Caught exception: " << e.what() << endl;
        cout << "ERROR(sparseskeletonpathplanner.cpp): while trying to connect start/goal to sparse skeleton." << endl;
        cout << "ERROR(sparseskeletonpathplanner.cpp): I didn't think this could happen, but I couldn't guarantee it..." << endl;
        cout << "ERROR(sparseskeletonpathplanner.cpp): Maybe we need to connect to the dense skeleton first." << endl;
        cout<<"TRACE(sparseskeletonpathplanner.cpp): startCell & goalCell: " << startCell << " & " << goalCell << endl;

        throw;
    }

    if ( startContiguousSparseSkel != goalContiguousSparseSkel )
        throw hydropathplan::PathDestinationUnreachableException( ERROR_INFO, "Start and end weren't connected by a contiguous skeleton." );

    //
    // 3. Plan along the sparse skeleton
    //
    std::vector<const SparseSkelNode*> nodePath;
    planAlongSparseSkel( *startContiguousSparseSkel,
                         *startNode, 
                         *goalNode,
                         nodePath );

    //
    // 4. Convert to the appropriate format
    //
    std::vector<Cell2D> startCells(1);
    startCells[0] = startCell;
    std::vector<Cell2D> goalCells(1);
    goalCells[0] = goalCell;
    convertToCellVector( startCells,
                         goalCells,
                         nodePath,
                         path );

    //
    // 5. Optionally optimise the path
    //
    if ( doPathOptimization_ )
        optimisePath( path );
}



}
