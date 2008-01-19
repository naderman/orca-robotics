/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "genericdriver.h"
#include <hydroutil/cpustopwatch.h>
#include <hydroutil/rand.h>
#include <orcaice/orcaice.h>
#include <iostream>

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
    
    double straightLineDist( const orca::Waypoint2d *wp1, const orca::Waypoint2d *wp2 )
    {
        double dX = wp1->target.p.x - wp2->target.p.x;
        double dY = wp1->target.p.y - wp2->target.p.y;
        return sqrt( dX*dX + dY*dY );
    }

}

GenericDriver::GenericDriver( hydropathplan::IPathPlanner2d  *pathPlanner,
                              const hydroogmap::OgMap        &ogMap,
                              double                         robotDiameterMetres,
                              double                         traversabilityThreshhold,
                              bool                           doPathOptimization,
                              bool                           jiggleWaypointsOntoClearCells,
                              const orcaice::Context         &context)
    : pathPlanner_(pathPlanner),
      ogMap_(ogMap),
      robotDiameterMetres_(robotDiameterMetres),
      traversabilityThreshhold_(traversabilityThreshhold),
      doPathOptimization_(doPathOptimization),
      jiggleWaypointsOntoClearCells_(jiggleWaypointsOntoClearCells),
      context_(context)
      
{
    if ( jiggleWaypointsOntoClearCells_ )
    {
        grownOgMap_ = ogMap_;
        assert( ogMap_.metresPerCellX() == ogMap_.metresPerCellY() );
        hydropathplan::util::growObstaclesOgMap( grownOgMap_,
                                                 traversabilityThreshhold_,
                                                 (int)(robotDiameterMetres_/grownOgMap_.metresPerCellX()) );
    }
}

GenericDriver::~GenericDriver()
{
}

void
GenericDriver::jiggleOntoClearCells( orca::Path2d &path )
{
    for ( unsigned int i=0; i < path.size(); i++ )
    {
        jiggleOntoClearCell( path[i], grownOgMap_, traversabilityThreshhold_ );
    }
}

void 
GenericDriver::computePath( const orca::PathPlanner2dTask& task,
                            orca::PathPlanner2dData& pathData )
{
    assert(task.coarsePath.size()!=0);

    const orca::Path2d *coarsePath = &(task.coarsePath);
    if ( jiggleWaypointsOntoClearCells_ )
    {
        jiggledPath_ = task.coarsePath;
        jiggleOntoClearCells( jiggledPath_ );
        coarsePath = &(jiggledPath_);
    }
    
    // get the first waypoint from the coarse path and save as startWp
    const orca::Waypoint2d *startWp = &((*coarsePath)[0]);
    const double firstHeading = startWp->target.o;
    
    // for each waypoint in the coarse path
    for (unsigned int i=1; i<coarsePath->size(); i++)
    {
        const orca::Waypoint2d *goalWp = &((*coarsePath)[i]);
        hydropathplan::Cell2DVector pathSegment;

        hydroutil::CpuStopwatch watch(true);
        assert(pathPlanner_!=0);
        try {
            int startX, startY, endX, endY;
            ogMap_.getCellIndices( startWp->target.p.x, startWp->target.p.y, startX, startY );
            ogMap_.getCellIndices( goalWp->target.p.x,  goalWp->target.p.y,  endX,   endY );
            pathPlanner_->computePath( startX,
                                       startY,
                                       endX,
                                       endY,
                                       pathSegment );
        }
        catch ( hydropathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error planning path segment from (" 
               << startWp->target.p.x <<","<<startWp->target.p.y << ") to ("
               << goalWp->target.p.x << ","<<goalWp->target.p.y<<"): "
               << e.what()
               << endl;

            // Warning: we lose the exception type information here...
            throw hydropathplan::Exception( ss.str() );
        }
        stringstream ss;
        ss << "Computing path segment took " << watch.elapsedSeconds() << "s";
        context_.tracer().debug(ss.str(),1);

        // ====== Set waypoint parameters ================================
        // ====== Different options could be implemented and chosen and runtime (via .cfg file)
        vector<orcapathplan::WaypointParameter> wpParaVector;
        setWaypointParameters( startWp, goalWp, pathSegment, ogMap_, wpParaVector );
        // ===============================================================

        // ===== Append to the pathData which contains the entire path  ========
        orcapathplan::convertAndAppend( ogMap_, pathSegment, wpParaVector, pathData, firstHeading );
        // ========================================================================
        
        // set last goal cell as new start cell
        startWp = goalWp;
    }
}

// This function has the following policy for adding waypoint parameters to the path consisting
// of a start point, intermediate points and an end point:
// (1) Start and end points use their settings as given
// (2) Intermediate points use the settings of the endpoint
// (3) Timing: timeSegment = timeTotal/lengthTotal * lengthSegment where
//        timeSegment:   time to travel current segment
//        timeTotal:     total time to get from start to goal
//        lengthTotal:   total length which is a sum of line lengths connecting all waypoints
//        lengthSegment: line length of the current segment
void
GenericDriver::setWaypointParameters( const orca::Waypoint2d                  *startWp, 
                                      const orca::Waypoint2d                  *goalWp, 
                                      const hydropathplan::Cell2DVector       &pathSegmentCells,
                                      const hydroogmap::OgMap                 &ogMap,
                                      vector<orcapathplan::WaypointParameter> &wpParaVector )
{
    wpParaVector.clear();
    
    // convert cell vector into slice data structure
    // needed for distance calculations
    orca::PathPlanner2dData pathSegmentSlice;
    orcapathplan::convertAndAppend( ogMap, pathSegmentCells, pathSegmentSlice );
    
    // compute the total length connecting all intermediate waypoints based on straight lines
    // store segment lengths in a vector for later
    assert( pathSegmentSlice.path.size()>1 );
    double lengthTotal=0.0;
    vector<double> lengthSegments;
    for (unsigned int i=1; i<pathSegmentSlice.path.size(); i++)
    {
        double lengthSegment = straightLineDist(&pathSegmentSlice.path[i-1], &pathSegmentSlice.path[i] );
        lengthSegments.push_back( lengthSegment );
        lengthTotal = lengthTotal + lengthSegment;
    }
    double timeTotal = orcaice::timeDiffAsDouble(goalWp->timeTarget, startWp->timeTarget);
    assert( timeTotal >= 0 && "Timestamp difference between goal and start is negative" );    
    double timeLengthRatio = timeTotal/lengthTotal;

    // add to waypoint parameters vector
    orcapathplan::WaypointParameter wpPara;
    for (unsigned int i=0; i<pathSegmentSlice.path.size(); i++)
    {
        if (i==0) 
        {
            wpPara.distanceTolerance = startWp->distanceTolerance;
            wpPara.maxApproachSpeed = startWp->maxApproachSpeed;
            wpPara.maxApproachTurnrate = startWp->maxApproachTurnrate;
            wpPara.timeTarget = orcaice::toOrcaTime( orcaice::timeAsDouble( startWp->timeTarget ) );
            wpPara.headingTolerance = startWp->headingTolerance;
        } 
        else 
        {
            wpPara.distanceTolerance = goalWp->distanceTolerance;
            wpPara.maxApproachSpeed = goalWp->maxApproachSpeed;
            wpPara.maxApproachTurnrate = goalWp->maxApproachTurnrate;
            // time for this segment is proportional to its length                        
            wpPara.timeTarget = orcaice::toOrcaTime( orcaice::timeAsDouble(wpParaVector[i-1].timeTarget) + timeLengthRatio * lengthSegments[i-1] );
            wpPara.headingTolerance = goalWp->headingTolerance;
        }
        wpParaVector.push_back( wpPara );
    }
}

}
