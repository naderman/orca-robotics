/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "driver.h"
#include <hydroutil/cpustopwatch.h>
#include <hydroutil/rand.h>
#include <orcaice/orcaice.h>
#include <iostream>
#include <orcaobj/stringutils.h>

using namespace std;

namespace pathplanner {

namespace {

    bool isClearWorld( double x,
                       double y,
                       const hydroogmap::OgMap &ogMap,
                       double traversabilityThreshhold )
    {
        unsigned char val;
        if( ogMap.tryWorldCell( x, y, val ) == true )
        {
            return ( val < (unsigned char) ((traversabilityThreshhold)*hydroogmap::CELL_OCCUPIED) );
        }
        return false;   
    }

    void jiggleOntoClearCell( orca::Waypoint2d &wp,
                              const hydroogmap::OgMap &ogMap,
                              double traversabilityThreshhold )
    {
        if ( isClearWorld( wp.target.p.x, wp.target.p.y, ogMap, traversabilityThreshhold ) )
            return;

        for ( int numSteps=2; numSteps < 10000; numSteps = (int)(numSteps*1.5) )
        {
            int xCell,yCell;
            ogMap.getCellIndices( wp.target.p.x, wp.target.p.y, xCell, yCell );
            hydropathplan::Cell2D cell(xCell,yCell);

            // perform a random walk of numSteps steps.
            for ( int i=0; i < numSteps; i++ )
            {
                cell = hydropathplan::surroundCell( cell, (int)(hydroutil::randNum(0,8)) );

                if ( hydropathplan::util::isTraversable( ogMap, cell.x(), cell.y(), traversabilityThreshhold ) )
                {
                    double worldX, worldY;
                    ogMap.getWorldCoords( cell.x(), cell.y(), worldX, worldY );
                    wp.target.p.x = worldX;
                    wp.target.p.y = worldY;
                    assert( isClearWorld( wp.target.p.x, wp.target.p.y, ogMap, traversabilityThreshhold ) );
                    return;
                }
            }
        }
        throw( hydropathplan::Exception( "Couldn't jiggle onto clear cell." ) );
    }
    
    double straightLineDist( const hydropathplan::Cell2D &c1,
                             const hydropathplan::Cell2D &c2,
                             const hydroogmap::OgMap     &ogMap )
    {
        double c1WorldX, c1WorldY;
        double c2WorldX, c2WorldY;
        ogMap.getWorldCoords( c1.x(), c1.y(), c1WorldX, c1WorldY );
        ogMap.getWorldCoords( c2.x(), c2.y(), c2WorldX, c2WorldY );

        return hypot( c2WorldY-c1WorldY, c2WorldX-c2WorldX );
    }

}

Driver::Driver( hydropathplan::IPathPlanner2d  &pathPlanner,
                const hydroogmap::OgMap        &ogMap,
                double                         robotDiameterMetres,
                double                         traversabilityThreshhold,
                bool                           jiggleWaypointsOntoClearCells,
                const orcaice::Context         &context)
    : pathPlanner_(pathPlanner),
      ogMap_(ogMap),
      traversabilityThreshhold_(traversabilityThreshhold),
      jiggleWaypointsOntoClearCells_(jiggleWaypointsOntoClearCells),
      context_(context)
      
{
    if ( jiggleWaypointsOntoClearCells_ )
    {
        grownOgMap_ = ogMap_;
        assert( ogMap_.metresPerCellX() == ogMap_.metresPerCellY() && "Can only deal with square cells" );
        // TODO: AlexB: why are we growing by the diameter, not the radius??
        hydropathplan::util::growObstaclesOgMap( grownOgMap_,
                                                 traversabilityThreshhold_,
                                                 (int)(robotDiameterMetres/grownOgMap_.metresPerCellX()) );
    }
}

void
Driver::jiggleOntoClearCells( orca::Path2d &path )
{
    for ( unsigned int i=0; i < path.size(); i++ )
    {
        jiggleOntoClearCell( path[i], grownOgMap_, traversabilityThreshhold_ );
    }
}

hydropathplan::Cell2DVector
Driver::computePathSegment( double startX,
                            double startY,
                            double endX,
                            double endY )
{
    int startCellX, startCellY, endCellX, endCellY;
    ogMap_.getCellIndices( startX, startY, startCellX, startCellY );
    ogMap_.getCellIndices( endX, endY, endCellX, endCellY );

    try {
        hydropathplan::Cell2DVector pathSegment;
        pathPlanner_.computePath( startCellX,
                                  startCellY,
                                  endCellX,
                                  endCellY,
                                  pathSegment );
        return pathSegment;
    }
    catch ( hydropathplan::Exception &e )
    {
        std::stringstream ss;
        ss << "While Planning path segment from (" 
           << startX <<","<< startY << ") to ("
           << endX << ","<< endY <<"): ";
        e.prependMsg( ss.str() );
        throw e;
    }    
}

void 
Driver::computePath( const orca::PathPlanner2dTask &task,
                     orca::Path2d                  &path )
{
    if ( task.coarsePath.size() == 0 )
    {
        stringstream ss;
        ss << "task.coarsePath.size() was zero.";
        throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    const orca::Path2d *coarsePath = &(task.coarsePath);
    if ( jiggleWaypointsOntoClearCells_ )
    {
        jiggledPath_ = task.coarsePath;
        jiggleOntoClearCells( jiggledPath_ );
        coarsePath = &(jiggledPath_);
    }

    // Add the first waypoint un-touched
    path.push_back( (*coarsePath)[0] );

    // for each segment in the coarse path
    for (unsigned int i=1; i<coarsePath->size(); i++)
    {
        const orca::Waypoint2d &startWp = (*coarsePath)[i-1];
        const orca::Waypoint2d &goalWp = (*coarsePath)[i];

        hydroutil::CpuStopwatch stopWatch(true);
        hydropathplan::Cell2DVector pathSegment = computePathSegment( startWp.target.p.x, 
                                                                      startWp.target.p.y,
                                                                      goalWp.target.p.x, 
                                                                      goalWp.target.p.y );
        stringstream ss;
        ss << "Driver: Computing path segment took " << stopWatch.elapsedSeconds() << "s";
        context_.tracer().debug(ss.str(),1);

        // Add the segment to pathData
        convertAndAppend( startWp, goalWp, pathSegment, ogMap_, path );
    }
}

// This function adds a waypoint for every cell bar the first.
// It has the following policy for orca::Waypoint2d parameters:
// (1) All parameters are set as per the end-point, with the exception of the heading and tolerance:
//       the end-point uses its params as specified, intermediate points use massive heading tolerance.
// (3) Timing: timeSegment = timeTotal/lengthTotal * lengthSegment where
//        timeSegment:   time to travel current segment
//        timeTotal:     total time to get from start to goal
//        lengthTotal:   total length which is a sum of line lengths connecting all waypoints
//        lengthSegment: line length of the current segment
void
Driver::convertAndAppend( const orca::Waypoint2d            &startWp, 
                          const orca::Waypoint2d            &goalWp,
                          const hydropathplan::Cell2DVector &pathCells,
                          const hydroogmap::OgMap           &ogMap,
                          orca::Path2d                      &path )
{
    assert( pathCells.size() >= 2 );
    assert( path.size() > 0 );
    
    // Compute the length of each leg of the trip
    std::vector<double> legLengths;
    double totalLength=0;
    for ( uint i=1; i < pathCells.size(); i++ )
    {
        double thisLength = straightLineDist( pathCells[i-1], pathCells[i], ogMap );
        thisLength += ogMap.metresPerCellX()/2.0; // avoid divide-by-zero
        legLengths.push_back( thisLength );
        totalLength += thisLength;
    }

    double timeTotal = orcaice::timeDiffAsDouble(goalWp.timeTarget, startWp.timeTarget);
    if ( timeTotal < 0 )
    {
        stringstream ss;
        ss << "Time of goal was _before_ time of start!" << endl
           << "  startWp.timeTarget: " << orcaobj::toString(startWp.timeTarget) << endl
           << "  goalWp.timeTarget:  " << orcaobj::toString(goalWp.timeTarget);
        throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    double timeLengthRatio = timeTotal/totalLength;

    // Add each leg to the orca-style path
    for ( uint i=1; i < pathCells.size(); i++ )
    {
        orca::Waypoint2d wp;

        if ( i < pathCells.size()-1 )
        {
            ogMap.getWorldCoords( pathCells[i].x(),
                                  pathCells[i].y(),
                                  wp.target.p.x,
                                  wp.target.p.y );
            wp.target.o   = 0;
            wp.headingTolerance = 2*M_PI;
        }
        else
        {
            wp.target = goalWp.target;
            wp.headingTolerance = goalWp.headingTolerance;
        }

        wp.distanceTolerance = goalWp.distanceTolerance;
        assert( (i-1) < legLengths.size() );
        wp.timeTarget = orcaice::toOrcaTime( orcaice::timeAsDouble(path.back().timeTarget) + timeLengthRatio * legLengths[i-1] );
        wp.maxApproachSpeed = goalWp.maxApproachSpeed;
        wp.maxApproachTurnrate = goalWp.maxApproachTurnrate;

        path.push_back( wp );
    }
}

}
