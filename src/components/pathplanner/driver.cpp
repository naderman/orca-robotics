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

    std::string toWorldString( const hydropathplan::Cell2D &c,
                               const hydroogmap::OgMap &ogMap )
    {
        stringstream ss;
        double cWorldX, cWorldY;
        ogMap.getWorldCoords( c.x(), c.y(), cWorldX, cWorldY );
        ss << "[" << cWorldX << ", " << cWorldY << "]";
        return ss.str();
    }

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

    double dist( const hydropathplan::Cell2D &c,
                 const orca::Frame2d         &f,
                 const hydroogmap::OgMap     &ogMap )
    {
        double cWorldX, cWorldY;
        ogMap.getWorldCoords( c.x(), c.y(), cWorldX, cWorldY );
        return hypot( f.p.y-cWorldY, f.p.x-cWorldX );
    }

    double dist( const orca::Frame2d         &f,
                 const hydropathplan::Cell2D &c,
                 const hydroogmap::OgMap     &ogMap )
    { return dist( c, f, ogMap ); }

    // Converts a discrete path through the OgMap (defined by at least two cells)
    // into a continuous set of waypoints.
    // The first cell represents 'start' and the last represents 'goal'.
    std::vector<orca::Waypoint2d>
    convert( const orca::Waypoint2d &start,
             const orca::Waypoint2d &goal,
             const hydropathplan::Cell2DVector &pathCells,
             const hydroogmap::OgMap &ogMap )
    {
        assert( pathCells.size() > 1 );

        // Make sure the start and end-points of pathCells coincide with 'start' and 'goal'
        if ( fabs(start.target.p.x - ogMap.worldXCoord(pathCells.front().x())) > ogMap.metresPerCellX() ||
             fabs(start.target.p.y - ogMap.worldYCoord(pathCells.front().y())) > ogMap.metresPerCellY() ||
             fabs(goal.target.p.x  - ogMap.worldXCoord(pathCells.back().x()))  > ogMap.metresPerCellX() ||
             fabs(goal.target.p.y  - ogMap.worldYCoord(pathCells.back().y()))  > ogMap.metresPerCellY() )
        {
            cout << "ERROR(driver.cpp): start and goal wps aren't on first and last cells." << endl;
            cout << "ERROR(driver.cpp): start: " << orcaobj::toString(start) << ", pathCells.front(): " << toWorldString(pathCells.front(),ogMap) << endl;
            cout << "ERROR(driver.cpp): goal:  " << orcaobj::toString(goal) << ", pathCells.back(): " << toWorldString(pathCells.back(),ogMap) << endl;
            assert( "bad path" && false );
        }

        // Compute the length of each leg of the trip
        std::vector<double> legLengths;
        double totalLength=0;
        for ( size_t i=1; i < pathCells.size(); i++ )
        {
            double thisLength;
            if ( i==1 )
                thisLength = dist( start.target, pathCells[i], ogMap );
            else if ( i == pathCells.size()-1 )
                thisLength = dist( pathCells[i-1], goal.target, ogMap );
            else
                thisLength = straightLineDist( pathCells[i-1], pathCells[i], ogMap );

            legLengths.push_back( thisLength );
            totalLength += thisLength;
        }

        double totalTime = orcaice::timeDiffAsDouble(goal.timeTarget, start.timeTarget);
        if ( totalTime < 0 )
        {
            stringstream ss;
            ss << "Time of goal was _before_ time of start!" << endl
               << "  startWp.timeTarget: " << orcaobj::toString(start.timeTarget) << endl
               << "  goalWp.timeTarget:  " << orcaobj::toString(goal.timeTarget);
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        // Add each leg to the orca-style path
        orca::Path2d orcaPath;
        for ( size_t i=0; i < pathCells.size(); i++ )
        {
            // For the start and goal, add the real thing
            if ( i==0 ) { orcaPath.push_back( start ); continue; }
            if ( i==pathCells.size()-1 ) { orcaPath.push_back( goal ); continue; }

            // Intermediate points
            orca::Waypoint2d wp;
            ogMap.getWorldCoords( pathCells[i].x(),
                                  pathCells[i].y(),
                                  wp.target.p.x,
                                  wp.target.p.y );
            wp.target.o   = 0;
            wp.headingTolerance = 2*M_PI;

            wp.distanceTolerance = goal.distanceTolerance;
            wp.maxApproachSpeed = goal.maxApproachSpeed;
            wp.maxApproachTurnrate = goal.maxApproachTurnrate;
            if ( totalTime == 0 )
            {
                wp.timeTarget = goal.timeTarget;
            }
            else
            {
                const double fractionOfTotalLength = legLengths[i-1]/totalLength;
                const double timeThisLeg = fractionOfTotalLength*totalTime;
                wp.timeTarget = orcaPath.back().timeTarget;
                orcaice::add( wp.timeTarget, timeThisLeg );
            }
            orcaPath.push_back( wp );
        }

        return orcaPath;
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
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
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
        hydropathplan::Cell2DVector pathCells = computePathSegment( startWp.target.p.x, 
                                                                    startWp.target.p.y,
                                                                    goalWp.target.p.x, 
                                                                    goalWp.target.p.y );
        stringstream ss;
        ss << "Driver: Computing path segment took " << stopWatch.elapsedSeconds() << "s";
        context_.tracer().debug(ss.str(),1);

        // Add the segment to pathData
        std::vector<orca::Waypoint2d> finePathSegment = convert( startWp, 
                                                                 goalWp,
                                                                 pathCells,
                                                                 ogMap_ );

        // Add to the path but ignore the first wp (it was the last wp of the previous segment)
        path.insert( path.end(), finePathSegment.begin()+1, finePathSegment.end() );
    }
}

}
