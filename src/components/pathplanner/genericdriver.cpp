/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "genericdriver.h"
#include <orcamisc/orcamisc.h>
#include <orcamisc/rand.h>
#include <orcaice/orcaice.h>
#include <iostream>

using namespace std;
using namespace orcaice;

namespace pathplanner {

namespace {

    bool isClearWorld( double x,
                       double y,
                       const orcaogmap::OgMap &ogMap,
                       double traversabilityThreshhold )
    {
        unsigned char val;
        if( ogMap.tryWorldCell( x, y, val ) == true )
        {
            return ( val < (unsigned char) ((traversabilityThreshhold)*orcaogmap::CELL_OCCUPIED) );
        }
        return false;   
    }

    void jiggleOntoClearCell( orca::Waypoint2d &wp,
                              const orcaogmap::OgMap &ogMap,
                              double traversabilityThreshhold )
    {
        if ( isClearWorld( wp.target.p.x, wp.target.p.y, ogMap, traversabilityThreshhold ) )
            return;

        for ( int numSteps=2; numSteps < 10000; numSteps = (int)(numSteps*1.5) )
        {
            int xCell,yCell;
            ogMap.getCellIndices( wp.target.p.x, wp.target.p.y, xCell, yCell );
            orcapathplan::Cell2D cell(xCell,yCell);

            // perform a random walk of numSteps steps.
            for ( int i=0; i < numSteps; i++ )
            {
                cell = orcapathplan::surroundCell( cell, (int)(orcamisc::randNum(0,8)) );

                if ( orcapathplan::isTraversable( ogMap, cell.x(), cell.y(), traversabilityThreshhold ) )
                {
                    float worldX, worldY;
                    ogMap.getWorldCoords( cell.x(), cell.y(), worldX, worldY );
                    wp.target.p.x = worldX;
                    wp.target.p.y = worldY;
                    assert( isClearWorld( wp.target.p.x, wp.target.p.y, ogMap, traversabilityThreshhold ) );
                    return;
                }
            }
        }
        throw( orcapathplan::Exception( "Couldn't jiggle onto clear cell." ) );
    }

}

GenericDriver::GenericDriver( orcapathplan::IPathPlanner2d  *pathPlanner,
                              const orcaogmap::OgMap        &ogMap,
                              double                         robotDiameterMetres,
                              double                         traversabilityThreshhold,
                              bool                           doPathOptimization,
                              bool                          jiggleWaypointsOntoClearCells,
                              const Context                 &context)
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
        orcapathplan::growObstaclesOgMap( grownOgMap_,
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
        orcapathplan::Cell2DVector pathSegment;

        orcamisc::CpuStopwatch watch(true);
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
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error planning path segment from (" 
               << startWp->target.p.x <<","<<startWp->target.p.y << ") to ("
               << goalWp->target.p.x << ","<<goalWp->target.p.y<<"): "
               << e.what()
               << endl;
            
            throw orcapathplan::Exception( ss.str(), e.type() );
        }
        stringstream ss;
        ss << "Computing path segment took " << watch.elapsedSeconds() << "s";
        context_.tracer()->debug(ss.str(),1);

        // ====== Set waypoint parameters ================================
        // ====== Different options could be implemented and chosen and runtime (via .cfg file)
        vector<orcapathplan::WaypointParameter> wpParaVector;
        setWaypointParameters( startWp, goalWp, pathSegment.size(), wpParaVector );
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
// (3) The time to reach intermediate points is divided linearly between start and end point times
void
GenericDriver::setWaypointParameters( const orca::Waypoint2d *startWp, 
                                       const orca::Waypoint2d *goalWp, 
                                       int numWaypoints,
                                       vector<orcapathplan::WaypointParameter> &wpParaVector )
{
    wpParaVector.clear();

    orcapathplan::WaypointParameter wpPara;
    double secondsTilGoal = orcaice::timeDiffAsDouble(goalWp->timeTarget, startWp->timeTarget);
    assert( secondsTilGoal >= 0 && "Timestamp difference between goal and start is negative" );
    double deltaSec = secondsTilGoal/(double)(numWaypoints-1);
            
    for (int i=0; i<numWaypoints; i++)
    {
        if (i==0) 
        {
            wpPara.distanceTolerance = startWp->distanceTolerance;
            wpPara.maxApproachSpeed = startWp->maxApproachSpeed;
            wpPara.maxApproachTurnrate = startWp->maxApproachTurnrate;
            wpPara.timeTarget = toOrcaTime( timeAsDouble( startWp->timeTarget ) );
            wpPara.headingTolerance = startWp->headingTolerance;
        } 
        else 
        {
            wpPara.distanceTolerance = goalWp->distanceTolerance;
            wpPara.maxApproachSpeed = goalWp->maxApproachSpeed;
            wpPara.maxApproachTurnrate = goalWp->maxApproachTurnrate;                        
            wpPara.timeTarget = toOrcaTime( timeAsDouble( wpParaVector[i-1].timeTarget ) + deltaSec );
            wpPara.headingTolerance = goalWp->headingTolerance;
        }
        wpParaVector.push_back( wpPara );
    }
}

}
