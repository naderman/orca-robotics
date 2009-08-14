/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "util.h"
#include <sstream>
#include <hydroogmap/oglostracer.h>

using namespace std;

namespace hydropathplan {
namespace util {

bool isTraversable( const hydroogmap::OgMap &ogMap,
                    const int indX,
                    const int indY,
                    const float traversabilityThreshhold)
{
    unsigned char val;
    if( ogMap.tryGridCell( indX, indY, val ) == true )
    {
        return ( val < (unsigned char) ((traversabilityThreshhold)*hydroogmap::CELL_OCCUPIED) );
    }
    return false;   
}

bool isTraversableNeighbors( const hydroogmap::OgMap &ogMap,
                             const int indX,
                             const int indY,
                             const float traversabilityThreshhold )
{
    return ( isTraversable(ogMap,indX+1,indY,traversabilityThreshhold) || 
             isTraversable(ogMap,indX-1,indY,traversabilityThreshhold) ||
             isTraversable(ogMap,indX,indY+1,traversabilityThreshhold) || 
             isTraversable(ogMap,indX,indY-1,traversabilityThreshhold) );
}

bool isTraversableAll8Neighbors( const hydroogmap::OgMap &ogMap,
                                 const int indX,
                                 const int indY,
                                 const float traversabilityThreshhold )
{
    return ( isTraversable(ogMap,indX+1,indY-1,traversabilityThreshhold) &&
             isTraversable(ogMap,indX+1,indY,traversabilityThreshhold)   && 
             isTraversable(ogMap,indX+1,indY+1,traversabilityThreshhold) && 
             isTraversable(ogMap,indX-1,indY-1,traversabilityThreshhold) &&
             isTraversable(ogMap,indX-1,indY,traversabilityThreshhold)   &&
             isTraversable(ogMap,indX-1,indY+1,traversabilityThreshhold) &&
             isTraversable(ogMap,indX,indY+1,traversabilityThreshhold)   &&
             isTraversable(ogMap,indX,indY-1,traversabilityThreshhold) );
}

bool losExists( const hydroogmap::OgMap &ogMap, double traversabilityThreshhold, const Cell2D &c1, const Cell2D &c2 )
{
    assert( ogMap.cellWithinMap( c1.x(), c1.y() ) );
    assert( ogMap.cellWithinMap( c2.x(), c2.y() ) );
    hydroogmap::OgLosTracer rayTracer( ogMap, (unsigned char)(traversabilityThreshhold*hydroogmap::CELL_OCCUPIED) );
    return rayTracer.isClearCells( c1.x(), c1.y(), c2.x(), c2.y() );
}


namespace {

    // Utility stuff for calculating straight-line costs using ray-tracing
    class CostAccumulator {
    public:
        CostAccumulator( const FloatMap &costMap )
            : costMap_(costMap),
              costSoFar_(0)
            {}

        inline bool traceCell( int x, int y )
            {
                costSoFar_ += element( costMap_, x, y );

                // keep tracing.
                return true;
            }

        inline void traceSucceeded( int x, int y ) {}

        double totalCost() const { return costSoFar_; }

    private:
        const FloatMap &costMap_;
        double costSoFar_;
    };
    typedef hydroogmap::RayTracer<CostAccumulator> CostCalculator;

    float oneWayCost( const Cell2D           &c1,
                      const Cell2D           &c2,
                      const FloatMap         &costMap )
    {
        CostAccumulator accum( costMap );
        CostCalculator costCalculator( accum );
        costCalculator.trace( c1.x(), c1.y(), c2.x(), c2.y() );
    
        double cost = accum.totalCost();

        // Assume we're tracing from centre-of-cell to centre-of-cell, so only count half the end-cells.
        cost -= element( costMap, c1 )/2.0;
        cost -= element( costMap, c2 )/2.0;

        return cost;
    }
}

float straightLineCost( const Cell2D           &c1,
                        const Cell2D           &c2,
                        const FloatMap         &costMap )
{
    // one-way cost may not be symmetric, so take the average of both ways.
    return ( oneWayCost(c1,c2,costMap) + oneWayCost(c1,c2,costMap) ) / 2.0;
}

int robotDiameterInCells( const hydroogmap::OgMap &ogMap,
                          const double             robotDiameterMetres )
{
    assert( robotDiameterMetres >= 0.0 && 
            robotDiameterMetres < ogMap.worldSizeX() && 
            robotDiameterMetres < ogMap.worldSizeY() );

    return (int) ceil( robotDiameterMetres / ogMap.metresPerCell() );
}

void growObstaclesOgMap( hydroogmap::OgMap &ogMap,
                         const double       traversabilityThreshhold,
                         const int          robotDiameterCells )
{

    Cell2DVector Lcurr;
    Cell2D q, q1;

    // efficient for large radii
    // find all the edges of the known obstacles
    for ( int i=0; i<ogMap.numCellsX(); i++ )
    {
        for ( int j=0; j<ogMap.numCellsY(); j++ )
        {
            if (  !isTraversable( ogMap, i,j, traversabilityThreshhold )  &&
                   isTraversableNeighbors( ogMap, i,j, traversabilityThreshhold ) )
            {
                assert( ogMap.cellWithinMap( i,j ) );
                Lcurr.push_back( Cell2D( i,j ) );
            }
        }
    }

    // extend them by robotDiameterCells cells
    for( int i=0; i<robotDiameterCells; i++ )
    {
        Cell2DVector Lnext;
        //cout << "Lcurr size: " << Lcurr.size()<<endl;
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            //cout << "Q: " << q << endl;
            Lcurr.pop_back();
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);

                // checking the boundaries
                if ( ogMap.cellWithinMap( q1.x(), q1.y() ) )
                {
                    const int thresh = (int)floor( (double)hydroogmap::CELL_OCCUPIED*traversabilityThreshhold );
                    if ( ogMap.gridCell( q1.x(), q1.y() ) < thresh )
                    {
                        ogMap.gridCell( q1.x(), q1.y() ) = hydroogmap::CELL_OCCUPIED;
                        Lnext.push_back( q1 );
                    }
                }
            }
        }
        Lcurr = Lnext;
    }
}

void optimizePath( const hydroogmap::OgMap &ogMap,
                   double                   traversabilityThreshhold,
                   const Cell2DVector      &origPath,
                   Cell2DVector            &optimisedPath )
{
    hydroogmap::OgLosTracer rayTracer( ogMap, (unsigned char)(traversabilityThreshhold*hydroogmap::CELL_OCCUPIED) );

#ifndef NDEBUG
    // Check that the original path is OK.
    if ( origPath.size() >= 2 )
    {
        for ( unsigned int i=1; i < origPath.size(); i++ )
        {
            Cell2D wp1 = origPath[i-1];
            Cell2D wp2 = origPath[i];
            if ( !rayTracer.isClearCells( wp1.x(), wp1.y(), wp2.x(), wp2.y() ) )
            {
                cout<<"TRACE(pathplanutils.cpp): OptimisePath: original path was messed up:" << endl;
                cout<<"TRACE(pathplanutils.cpp): Couldn't get from wp "<<i-1<<" to "<<i<< endl;
                cout<<"TRACE(pathplanutils.cpp):   " << wp1 << " --> " << wp2 << endl;
                cout<<"TRACE(pathplanutils.cpp): entire origPath: " << endl;
                for ( unsigned int j=0; j < origPath.size(); j++ )
                {
                    cout << "  " << j << ": " << origPath[j] << endl;
                }
                assert( false && "optimizePath received dodgy input path!" );
            }
        }
    }
#endif


    assert( origPath.size() > 0 );

    optimisedPath.push_back( origPath[0] );
    if ( origPath.size() >= 2 )
    {

        int priorI  = 0;
        int middleI = 1;

        //
        // In this while loop: middle and post are always contiguous wps from origPath.
        //
        while ( (middleI+1) < (int)(origPath.size()) )
        {
            const Cell2D &prior  = origPath[priorI];
            const Cell2D &middle = origPath[middleI];
            const Cell2D &post   = origPath[middleI+1];

#ifndef NDEBUG
            if ( !rayTracer.isClearCells( prior.x(), prior.y(), middle.x(), middle.y() ) )
            {
                cout << "ERROR(pathplanutils.cpp): Couldn't trace from prior to middle." << endl;
                cout<<"TRACE(pathplanutils.cpp): prior:  " << prior << endl;
                cout<<"TRACE(pathplanutils.cpp): middle: " << middle << endl;
                cout<<"TRACE(pathplanutils.cpp): post:   " << post << endl;
                cout<<"TRACE(pathplanutils.cpp): origPath: " << endl;
                for ( unsigned int j=0; j < origPath.size(); j++ )
                    cout << "  " << j << ": " << origPath[j] << endl;
                assert( false );
            }
#endif

            if ( !rayTracer.isClearCells( prior.x(), prior.y(), post.x(), post.y() ) )
            {
                // There's a direct line from prior to middle, but _not_ from prior to post.
                // Therefore the middle-man is necessary.
                optimisedPath.push_back( middle );
                priorI  = middleI;
            }
            else
            {
                // skip the middle-man
            }
            middleI++;
        }
    }

    // Add the last cell
    optimisedPath.push_back( origPath.back() );

#ifndef NDEBUG
    // Just double-check that the path is OK.
    if ( optimisedPath.size() < 2 ) return;
    for ( unsigned int i=1; i < optimisedPath.size(); i++ )
    {
        Cell2D wp1 = optimisedPath[i-1];
        Cell2D wp2 = optimisedPath[i];
        if ( !rayTracer.isClearCells( wp1.x(), wp1.y(), wp2.x(), wp2.y() ) )
        {
            cout<<"TRACE(pathplanutils.cpp): OptimisePath messed up:" << endl;
            cout<<"TRACE(pathplanutils.cpp): Couldn't get from wp "<<i-1<<" to "<<i<< endl;
            cout<<"TRACE(pathplanutils.cpp):   " << wp1 << " --> " << wp2 << endl;
            cout<<"TRACE(pathplanutils.cpp): entire optimised path: " << endl;
            for ( unsigned int j=0; j < optimisedPath.size(); j++ )
            {
                cout << "  " << j << ": " << optimisedPath[j] << endl;
            }
            cout<<"TRACE(pathplanutils.cpp): origPath: " << endl;
            for ( unsigned int j=0; j < origPath.size(); j++ )
            {
                cout << "  " << j << ": " << origPath[j] << endl;
            }
            assert( false && "optimizePath messed up!" );
        }
    }
#endif
}

}
}
