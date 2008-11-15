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

//     std::string toWorldString( const hydropathplan::Cell2D &c,
//                                const hydroogmap::OgMap &ogMap )
//     {
//         stringstream ss;
//         double cWorldX, cWorldY;
//         ogMap.getWorldCoords( c.x(), c.y(), cWorldX, cWorldY );
//         ss << "[" << cWorldX << ", " << cWorldY << "]";
//         return ss.str();
//     }

//     bool isClearWorld( double x,
//                        double y,
//                        const hydroogmap::OgMap &ogMap,
//                        double traversabilityThreshhold )
//     {
//         unsigned char val;
//         if( ogMap.tryWorldCell( x, y, val ) == true )
//         {
//             return ( val < (unsigned char) ((traversabilityThreshhold)*hydroogmap::CELL_OCCUPIED) );
//         }
//         return false;   
//     }

    double straightLineDist( const hydrointerfaces::PathPlanner2d::Point &p1,
                             const hydrointerfaces::PathPlanner2d::Point &p2 )
    {
        return hypot( p1.y-p2.y, p1.x-p2.x );
    }

    double dist( const hydrointerfaces::PathPlanner2d::Point &p,
                 const orca::Frame2d                         &f )
    {
        return hypot( f.p.y-p.y, f.p.x-p.x );
    }

    double dist( const orca::Frame2d                         &f,
                 const hydrointerfaces::PathPlanner2d::Point &p )
    { return dist( p, f ); }

    // Converts a path (defined by at least two waypoints)
    // into a set of Orca waypoints.
    // The first cell represents 'start' and the last represents 'goal'.
    std::vector<orca::Waypoint2d>
    convert( const orca::Waypoint2d &start,
             const orca::Waypoint2d &goal,
             const vector<hydrointerfaces::PathPlanner2d::Point> &pathPoints,
             double intermediateWaypointMinDistTolerance )
    {
        assert( pathPoints.size() > 1 );

        // Compute the length of each leg of the trip
        std::vector<double> legLengths;
        double totalLength=0;
        for ( size_t i=1; i < pathPoints.size(); i++ )
        {
            double thisLength;
            if ( i==1 )
                thisLength = dist( start.target, pathPoints[i] );
            else if ( i == pathPoints.size()-1 )
                thisLength = dist( pathPoints[i-1], goal.target );
            else
                thisLength = straightLineDist( pathPoints[i-1], pathPoints[i] );

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
        for ( size_t i=0; i < pathPoints.size(); i++ )
        {
            // For the start and goal, add the real thing
            if ( i==0 ) { orcaPath.push_back( start ); continue; }
            if ( i==pathPoints.size()-1 ) { orcaPath.push_back( goal ); continue; }

            // Intermediate points
            orca::Waypoint2d wp;
            wp.target.p.x = pathPoints[i].x;
            wp.target.p.y = pathPoints[i].y;
            wp.target.o   = 0;
            wp.headingTolerance = 2*M_PI;

            wp.distanceTolerance = intermediateWaypointMinDistTolerance;
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

Driver::Driver( hydrointerfaces::PathPlanner2d &hydroDriver,
//                 double                         robotDiameterMetres,
//                 double                         traversabilityThreshhold,
                double                         intermediateWaypointMinDistTolerance,
                const orcaice::Context         &context)
    : hydroDriver_(hydroDriver),
//       traversabilityThreshhold_(traversabilityThreshhold),
      intermediateWaypointMinDistTolerance_(intermediateWaypointMinDistTolerance),
      context_(context)      
{
}

std::vector<hydrointerfaces::PathPlanner2d::Point>
Driver::computePathSegment( double startX,
                            double startY,
                            double endX,
                            double endY )
{
    try {
        std::vector<hydrointerfaces::PathPlanner2d::Point> pathSegment =
        hydroDriver_.computePath( hydrointerfaces::PathPlanner2d::Point(startX,startY),
                                  hydrointerfaces::PathPlanner2d::Point(endX,endY) );
        return pathSegment;
    }
    catch ( hydrointerfaces::PathPlanner2d::Exception &e )
    {
        std::stringstream ss;
        ss << "While Planning path segment from (" 
           << startX <<","<< startY << ") to ("
           << endX << ","<< endY <<"): ";
        e.prependMsg( ERROR_INFO, ss.str() );
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

    // Add the first waypoint un-touched
    path.push_back( (*coarsePath)[0] );

    // for each segment in the coarse path
    for (unsigned int i=1; i<coarsePath->size(); i++)
    {
        const orca::Waypoint2d &startWp = (*coarsePath)[i-1];
        const orca::Waypoint2d &goalWp = (*coarsePath)[i];

        hydroutil::CpuStopwatch stopWatch(true);
        vector<hydrointerfaces::PathPlanner2d::Point> pathSegment = computePathSegment( startWp.target.p.x, 
                                                                                        startWp.target.p.y,
                                                                                        goalWp.target.p.x, 
                                                                                        goalWp.target.p.y );
        stringstream ss;
        ss << "Driver: Computing path segment took " << stopWatch.elapsedSeconds() << "s";
        context_.tracer().debug(ss.str(),1);

        // Add the segment to pathData
        std::vector<orca::Waypoint2d> finePathSegment = convert( startWp, 
                                                                 goalWp,
                                                                 pathSegment,
                                                                 intermediateWaypointMinDistTolerance_ );

        // Add to the path but ignore the first wp (it was the last wp of the previous segment)
        path.insert( path.end(), finePathSegment.begin()+1, finePathSegment.end() );
    }
}

}
