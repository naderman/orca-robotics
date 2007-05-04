/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "skeletonpathplanner.h"
#include "pathplanutils.h"
#include <orcamisc/orcamisc.h>
#include <iostream>

using namespace std;

namespace orcapathplan {

SkeletonPathPlanner::SkeletonPathPlanner( const orcaogmap::OgMap &ogMap,
                                          double robotDiameterMetres,
                                          double traversabilityThreshhold,
                                          bool   doPathOptimization )
    : ogMap_(ogMap),
      robotDiameterMetres_(robotDiameterMetres),
      traversabilityThreshhold_(traversabilityThreshhold),
      doPathOptimization_(doPathOptimization)
{
// Un-comment to see OgMap as text.    
//     cout<<"TRACE(skeletonpathplanner.cpp): ogMap: " << endl;
//     cout << orcaogmap::toText( ogMap ) << endl;

    assert( robotDiameterMetres >= 0.0 );
    assert( traversabilityThreshhold >= 0.0 );

    // Measuring performance
    orcamisc::CpuStopwatch watch;

    watch.start();
    bool success = computeSkeleton( ogMap_,
                                    skel_,
                                    distGrid_,
                                    traversabilityThreshhold,
                                    robotDiameterMetres );

    computeCostsFromDistGrid( distGrid_,
                              costMap_,
                              ogMap_.metresPerCellX() );

    watch.stop();
    cout << "TRACE(skeletonpathplanner.cpp): computeSkeleton took " << watch.elapsedSeconds() << "s" << endl;
    
    if ( !success )
    {
        throw orcapathplan::Exception( "Failed to compute skeleton over OG Map" );
    }
}

void 
SkeletonPathPlanner::checkInputs( int           startX,
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
SkeletonPathPlanner::computePath( int           startX,
                                  int           startY,
                                  int           endX,
                                  int           endY,
                                  Cell2DVector &path ) const
{
    checkInputs( startX, startY, endX, endY );
    Cell2D startCell( startX, startY );
    Cell2D goalCell(  endX,   endY   );

    orcamisc::CpuStopwatch watch;
    watch.start();

    Cell2DVector tempSkel = skel_;

    int robotDiameterInCells = orcapathplan::robotDiameterInCells( ogMap_, robotDiameterMetres_ );
    if ( !connectCell2Skeleton( tempSkel,
                                startCell,
                                distGrid_,
                                robotDiameterInCells ) )
    {
        throw orcapathplan::PathStartNotValidException( "Couldn't connect start cell to skeleton" );
    }

    watch.stop();
    cout<<"TRACE(skeletonpathplanner.cpp): connectCell2Skeleton(start) took " << watch.elapsedSeconds() << "s" << endl;

    watch.start();
    if ( !connectCell2Skeleton( tempSkel,
                                goalCell,
                                distGrid_,
                                robotDiameterInCells ) )
    {
        throw orcapathplan::Exception( "Couldn't connect skeleton to goal" );
    }

    watch.stop();
    cout<<"TRACE(skeletonpathplanner.cpp): connectCell2Skeleton(goal) took " << watch.elapsedSeconds() << "s" << endl;

    // Compute potential function U along the skeleton
    watch.start();
    FloatMap navMap;
    if( !computePotentialSkeleton( ogMap_, costMap_, navMap, tempSkel, startCell ) )
    {
        throw orcapathplan::Exception( "Couldn't compute potential function along skeleton" );
    }
    watch.stop();
    cout << "TRACE(skeletonpathplanner.cpp): computePotentialSkeleton took " << watch.elapsedSeconds() << " s" << endl << endl;

    // Compute potential function U in the free space

    // ===================================================

    // =========== Compute path ========================
    watch.start();
    Result result = orcapathplan::calcPath( ogMap_, navMap, goalCell, path, robotDiameterMetres_ );
    watch.stop();
    cout << "TRACE(skeletonpathplanner.cpp): Calculating path took: " << watch.elapsedSeconds() << " s" << endl << endl;

    if ( result!=PathOk )
    {
        throw orcapathplan::Exception( "Computed potential function but could not compute path." );
    }
    // =================================================
    
    // ============= Optional path optimization ================

    if ( doPathOptimization_ )
    {
        // separate full path into a optimized short path
        Cell2DVector waycells;    
        optimizePath( ogMap_, costMap_, traversabilityThreshhold_, path, waycells );
        path = waycells;
    }

    // =========================================================
}

}
