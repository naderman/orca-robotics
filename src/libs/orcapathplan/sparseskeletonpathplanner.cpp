/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "sparseskeletonpathplanner.h"
#include <orcamisc/orcamisc.h>
#include <iostream>

using namespace std;

namespace orcapathplan {

SparseSkeletonPathPlanner::SparseSkeletonPathPlanner( const orcaogmap::OgMap &ogMap,
                                                      double robotDiameterMetres,
                                                      double traversabilityThreshhold,
                                                      bool   doPathOptimization )
    : ogMap_(ogMap),
      robotDiameterMetres_(robotDiameterMetres),
      traversabilityThreshhold_(traversabilityThreshhold),
      doPathOptimization_(doPathOptimization)
{
    assert( traversabilityThreshhold >= 0.0 && traversabilityThreshhold <= 1.0 );

    // Measuring performance
    orcamisc::CpuStopwatch watch;

    watch.start();

    //
    // Build the dense skeleton
    //

    FloatMap distGrid;
    bool success = computeSkeleton( ogMap_,
                                    skel_,
                                    distGrid,
                                    traversabilityThreshhold,
                                    robotDiameterMetres );

    computeCostsFromDistGrid( distGrid,
                              costMap_,
                              ogMap_.metresPerCellX() );

    watch.stop();
    cout << "TRACE(skeletonpathplanner.cpp): computeSkeleton took " << watch.elapsedSeconds() << "s" << endl;
    
    if ( !success )
    {
        throw orcapathplan::Exception( "Failed to compute skeleton over OG Map" );
    }

    watch.start();

    //
    // convert to sparse skeleton
    //

    // first grow obstacles

    grownOgMap_ = ogMap_;

    // Maybe a bit of a hack...
    // Grow by half the robot diameter - 1, to avoid the possibility of growing over the dense skeleton.
    int robotDiameterCells = robotDiameterInCells( grownOgMap_, robotDiameterMetres );
    growObstaclesOgMap( grownOgMap_, traversabilityThreshhold, robotDiameterCells/2-1 );

    planOgMap_ = &(grownOgMap_);

    // cout<<"TRACE(sparseskeletonpathplanner.cpp): planOgMap_: " << endl << orcaogmap::toText(*planOgMap_) << endl;

    sparseSkel_ = new SparseSkel( (*planOgMap_), traversabilityThreshhold_, skel_ );
}

void 
SparseSkeletonPathPlanner::checkInputs( int           startX,
                                        int           startY,
                                        int           endX,
                                        int           endY ) const
{
    unsigned char val;

    if( ogMap_.tryGridCell( startX, startY, val ) == false )
        throw orcapathplan::Exception( "Start point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::Exception( "Start point was not traversable." );

    if ( grownOgMap_.gridCell(startX,startY) > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::Exception( "Robot can't fit on start point (too close to obstacle)." );

    if( ogMap_.tryGridCell( endX, endY, val ) == false )
        throw orcapathplan::Exception( "End point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::Exception( "End point was not traversable." );

    if ( grownOgMap_.gridCell(endX,endY) > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::Exception( "Robot can't fit on end point (too close to obstacle)." );
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
            float thisDist = hypotf( nodes[j]->pos.x()-cell.x(),
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

        if ( losExists( (*planOgMap_), traversabilityThreshhold_, cellOnDenseSkel, node->pos ) )
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
    throw orcapathplan::Exception( ss.str() );
}

void
SparseSkeletonPathPlanner::planAlongSparseSkel( const ContiguousSparseSkel   &contiguousSparseSkel,
                                                const SparseSkelNode         &startNode, 
                                                const SparseSkelNode         &goalNode,
                                                std::vector<const SparseSkelNode*> &nodePath ) const
{
    // cout<<"----------- planAlongSparseSkel() -------------" << endl;
    orcapathplan::findShortestPath( contiguousSparseSkel,
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
        float distFull = distance( startCells.back(), nodePath[0]->pos ) + 
                         distance( nodePath[0]->pos, nodePath[1]->pos );
        float distCut  = distance( startCells.back(), nodePath[1]->pos );
        if ( distCut < distFull &&
             losExists( (*planOgMap_), traversabilityThreshhold_, startCells.back(), nodePath[1]->pos ) )
                firstNode = 1;
    }

    // Do we need the last node?
    int lastNode = nodePath.size()-1;
    if ( nodePath.size() - firstNode > 1 )
    {
        float distFull = distance( nodePath[nodePath.size()-2]->pos, nodePath[nodePath.size()-1]->pos ) + 
                         distance( nodePath[nodePath.size()-1]->pos, goalCells[0] );
        float distCut  = distance( nodePath[nodePath.size()-2]->pos, goalCells[0] );
        if ( distCut < distFull &&
             losExists( (*planOgMap_), traversabilityThreshhold_, nodePath[nodePath.size()-2]->pos, goalCells[0] ) )
                lastNode = nodePath.size()-2;
    }

    // Is it even worth going via the skeleton?
    if ( lastNode-firstNode == 0 &&
         losExists( (*planOgMap_), traversabilityThreshhold_, startCells.back(), goalCells.front() ) )
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
        assert( losExists( *planOgMap_, traversabilityThreshhold_, path[i], path[i+1] ) );
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
    orcapathplan::optimizePath( grownOgMap_, costMap_, traversabilityThreshhold_, path, optimisedPath );
    path = optimisedPath;
}

void 
SparseSkeletonPathPlanner::computePath( int           startX,
                                        int           startY,
                                        int           endX,
                                        int           endY,
                                        Cell2DVector &path ) const
{
    try {

        // cout << "=============== computePath =================" << endl;

        // First connect the endpoints to the dense skel
        checkInputs( startX, startY, endX, endY );
        Cell2D startCell( startX, startY );
        Cell2D goalCell(  endX,   endY   );
    
        orcamisc::CpuStopwatch watch;
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
        catch ( orcapathplan::Exception &e )
        {
            cout << "ERROR(sparseskeletonpathplanner.cpp): Caught exception: " << e.what() << endl;
            cout << "ERROR(sparseskeletonpathplanner.cpp): while trying to connect start/goal to sparse skeleton." << endl;
            cout << "ERROR(sparseskeletonpathplanner.cpp): I didn't think this could happen, but I couldn't guarantee it..." << endl;
            cout << "ERROR(sparseskeletonpathplanner.cpp): Maybe we need to connect to the dense skeleton first." << endl;
            throw;
        }

        if ( startContiguousSparseSkel != goalContiguousSparseSkel )
            throw orcapathplan::Exception( "Start and end weren't connected by a contiguous skeleton." );

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
    catch ( orcapathplan::Exception &e )
    {
        // Just add extra diagnostics and re-throw.
        std::stringstream ss;
        ss << "While planning from cell ("<<startX<<","<<startY<<") to ("<<endX<<","<<endY<<"): "<<e.what();
        throw orcapathplan::Exception( ss.str() );
    }
}



}
