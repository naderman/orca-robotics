/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "simplenavpathplanner.h"
#include <hydropathplan/latombeutil.h>
#include <hydroutil/cpustopwatch.h>
#include <iostream>

using namespace std;

namespace hydropathplan {

namespace {

// Modifies navMap: grows obstacles using the diameter of the robot (obstacles are represented by NANs)
void growObstaclesNavMap( const hydroogmap::OgMap &ogMap,
                          FloatMap               &navMap,
                          double                  traversabilityThreshhold,
                          const double            robotDiameterMetres )
{
    Cell2DVector Lcurr,Lnext,Lold;
    Cell2D q, q1;

    // efficient for large radii
    // find all the edges of the known obstacles
    for ( int i=0; i<ogMap.numCellsX(); i++ )
    {
        for ( int j=0; j<ogMap.numCellsY(); j++ )
        {
            if (  !util::isTraversable( ogMap, i,j, traversabilityThreshhold )  &&
                   util::isTraversableNeighbors( ogMap, i,j, traversabilityThreshhold ) )
            {
                Lcurr.push_back( Cell2D( i,j ) );
            }
        }
    }

    int robotDiameterCells = util::robotDiameterInCells( ogMap, robotDiameterMetres );

    // extend them by robotDiameterCells cells
    for( int i=0; i<robotDiameterCells; i++ )
    {
        //cout << "Lcurr size: " << Lcurr.size()<<endl;
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            //cout << "Q: " << q << endl;
            Lcurr.pop_back();
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);
                if ( !latombeutil::containsNan( navMap, q1 ) ) //selects traversable cell around q1
                {
                    navMap.setElement( q1, NAN );
                    Lnext.push_back( q1 );
                }
            }
        }
        Lcurr.swap(Lnext);
    }    
}



bool
calcSimpleNavigation( const hydroogmap::OgMap  &ogMap,
                      FloatMap                 &navMap,
                      Cell2D                   &startCell,
                      const double              traversabilityThreshhold,
                      const double              robotDiameterMetres )
{

    if ( ogMap.numCellsX()==0 || ogMap.numCellsY()==0 ) {
        cout << "ERROR(pathplanutils.cpp): calcSimpleNavigation: OgMap is empty along one dimension at least. Returning..." << endl;
        return false;
    }

    // initialize navMap
    navMap.resize( ogMap.numCellsX(), ogMap.numCellsY() );
    navMap.fill( NAN );

    //M represents a large potential which must be at least the size of n-neighbours of the goal cell where n is the max number of goal cell neighbours in the world. We set it to twice the worldsize.
    const double M = 2.0 * ogMap.numCellsX() * ogMap.numCellsY();
    
    int counter = 0;
    // for every q in GCfree do U(q)=M (large number)
    for ( unsigned int i=0; i<navMap.sizeX(); i++ )
    {
        for ( unsigned int j=0; j<navMap.sizeY(); j++ )
        {
            if ( util::isTraversable( ogMap, i,j, traversabilityThreshhold ) ) {
                navMap.setElement(i,j, M );
                counter ++;
            }
        }
    }


    // current and next list of cells
    Cell2DVector Lcurr,Lnext;

    // current and next cells
    Cell2D q, q1;

    
    // grow obstacles to robot diameter (extends NaN of utility grid)
    hydroutil::CpuStopwatch watch;
    watch.start();
    growObstaclesNavMap( ogMap, navMap, traversabilityThreshhold, robotDiameterMetres );
    watch.stop();
    cout << "growObstaclesNavMap took " << watch.elapsedSeconds() << " s" << endl << endl;

    //check current cell is sane (and shift it slightly if it isn't)
    if( !latombeutil::validateCell( navMap, startCell, util::robotDiameterInCells(ogMap, robotDiameterMetres), true ) ) {
        cout << "ERROR(pathplanutils.cpp): calcSimpleNavigation: startCell not valid. Returning..." << endl;
        return false;
    }

    // set utility of the start point to zero
    // U(q_origin)=0, initialize current list, insert q_origin into Lo
    navMap.setElement(startCell, 0.0);
    Lcurr.push_back( startCell );

    // flags used to determine if diagonal cell is reachable
    bool prev;
    bool first;

    // for i=0,1,..., until Li (Lnext) is empty
    while ( !Lcurr.empty() )
    {
        // for every q in Li starting at back
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            Lcurr.pop_back();

            // reset falgs
            prev=false;
            first=false;

            // for every 1-neighbor q' of q in GC_free
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q,k);
                if( !latombeutil::containsNan(navMap,q1) ) // i.e. is traversable and not too close to obstacle
                {
                    if( element(navMap, q1 ) > element(navMap, q ) + latombeutil::AC ) //+ OCCUP_PENALTY*mq2
                    {
                        navMap.setElement( q1, element(navMap, q ) + latombeutil::AC ); //+ OCCUP_PENALTY*mq2
                        Lnext.push_back( q1 );
                    }
                    // check diagonal
                    q1 = diagonalCell(q,k);
                    if( prev && !latombeutil::containsNan(navMap,q1) )  // other cells are traversable, diagonal is reachable
                    {
                        if( element(navMap, q1 ) > element(navMap, q ) + latombeutil::DC )  //+ OCCUP_PENALTY*mq2
                        {
                            navMap.setElement( q1, element(navMap, q ) + latombeutil::DC ); //+ OCCUP_PENALTY*mq2
                            if( !isIncluded(Lnext,q1) )
                            {
                                Lnext.push_back( q1 );
                            }
                        }
                    }
                    // separate test for diagonal when k=0
                    if( k==3 )
                    {
                        q1 = diagonalCell(q,0);
                        if( first && !latombeutil::containsNan(navMap,q1) )  // other cells are traversable, diagonal is reachable
                        {
                            if( element(navMap, q1 ) > element(navMap, q ) + latombeutil::DC )  //+ OCCUP_PENALTY*mq2
                            {
                                //update cell
                                navMap.setElement( q1, element(navMap, q ) + latombeutil::DC ); //+ OCCUP_PENALTY*mq2
                                if( !isIncluded(Lnext,q1) )
                                {
                                    Lnext.push_back( q1 );
                                }
                            }
                        }
                    }
                    prev=true;
                    if( k==0 ) first=true;
                }
                else //not traversable
                {
                    prev=false;
                    // don't need: if( k==0 ) first=false; since flag will stay false
                }
            }
        }
        Lcurr.swap(Lnext);
    }
    return true;
}    

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

SimpleNavPathPlanner::SimpleNavPathPlanner( const hydroogmap::OgMap &ogMap,
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
        throw hydropathplan::PathStartNotValidException( ERROR_INFO, "Start point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw hydropathplan::PathStartNotValidException( ERROR_INFO, "Start point was not traversable." );

    if( ogMap_.tryGridCell( endX, endY, val ) == false )
        throw hydropathplan::PathDestinationNotValidException( ERROR_INFO, "End point was not within the map." );
    if ( val > (unsigned char) ((traversabilityThreshhold_)*254.0) )
        throw hydropathplan::PathDestinationNotValidException( ERROR_INFO, "End point was not traversable." );
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
        throw hydropathplan::Exception( ERROR_INFO, "SimpleNavPathPlanner::computePath: Navigation function could not be computed" );
    }
    // ===================================================
    
    // =========== Compute path ========================
    cout << "INFO(simplenavpathplanner.cpp): Calculating path now" << endl;
    watch.start();
    latombeutil::Result result = latombeutil::calcPath( ogMap_, navMap, goalCell, path, robotDiameterMetres_ );
    watch.stop();
    cout << "INFO(simplenavpathplanner.cpp): Calculating path took " << watch.elapsedSeconds() << " s" << endl;

    if ( result!=latombeutil::PathOk )
    {
        throw hydropathplan::Exception( ERROR_INFO, "Path could not be computed" );
    }
    // =================================================
    
    //============= Optional path optimization ================
    if ( doPathOptimization_ )
    {
        Cell2DVector waycells;    
        util::optimizePath( ogMap_, traversabilityThreshhold_, path, waycells );
        path = waycells;
    }
    //=========================================================
}

}
