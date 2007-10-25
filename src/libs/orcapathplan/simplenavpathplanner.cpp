/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "simplenavpathplanner.h"
#include "pathplanutils.h"
#include <hydroutil/cpustopwatch.h>
#include <iostream>

using namespace std;

namespace orcapathplan {

SimpleNavPathPlanner::SimpleNavPathPlanner( const orcaogmap::OgMap &ogMap,
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
}

void 
SimpleNavPathPlanner::checkInputs( int          startX,
                                  int           startY,
                                  int           endX,
                                  int           endY ) const
{
    unsigned char val;

    if( ogMap_.tryGridCell( startX, startY, val ) == false )
        throw orcapathplan::PathStartNotValidException( "Start point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::PathStartNotValidException( "Start point was not traversable." );

    if( ogMap_.tryGridCell( endX, endY, val ) == false )
        throw orcapathplan::PathDestinationNotValidException( "End point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw orcapathplan::PathDestinationNotValidException( "End point was not traversable." );
}

void
SimpleNavPathPlanner::computePath( int          startX,
                                  int           startY,
                                  int           endX,
                                  int           endY,
                                  Cell2DVector &path ) const
{
    cout << "INFO(simplenavpathplanner.cpp): computePath" << endl;
    
    checkInputs( startX, startY, endX, endY );
    Cell2D startCell( startX, startY );
    Cell2D goalCell(  endX,   endY   );

    hydroutil::CpuStopwatch watch;
    FloatMap navMap;
    bool success;
    FloatMap distGrid;
    
    // ============ Compute navigation function ========= 
    cout << "INFO(simplenavpathplanner.cpp): Starting to calculate navigation function" << endl;
    
    watch.start();
    success = calcSimpleNavigation( ogMap_, navMap, startCell, traversabilityThreshhold_, robotDiameterMetres_ );
    watch.stop();
    cout << "INFO(simplenavpathplanner.cpp): Calculating navigation fct took " << watch.elapsedSeconds() << " s" << endl;
    
    if ( !success )
    {
        throw orcapathplan::Exception( "Navigation function could not be computed" );
    }
    // ===================================================
    
    // =========== Compute path ========================
    cout << "INFO(simplenavpathplanner.cpp): Calculating path now" << endl;
    watch.start();
    Result result = orcapathplan::calcPath( ogMap_, navMap, goalCell, path, robotDiameterMetres_ );
    watch.stop();
    cout << "INFO(simplenavpathplanner.cpp): Calculating path took " << watch.elapsedSeconds() << " s" << endl;

    if ( result!=PathOk )
    {
        throw orcapathplan::Exception( "Path could not be computed" );
    }
    // =================================================
    
    //============= Optional path optimization ================
    if ( doPathOptimization_ )
    {
        Cell2DVector waycells;    
        optimizePath( ogMap_, traversabilityThreshhold_, path, waycells );
        path = waycells;
    }
    //=========================================================
}

}
