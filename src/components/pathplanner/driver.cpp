/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include <orcaobj/datetime.h>

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

    double dist( const hydrointerfaces::PathPlanner2d::Point &p1,
                 const hydrointerfaces::PathPlanner2d::Point &p2 )
    { return hypot( p1.y-p2.y, p1.x-p2.x ); }

    // Converts a path (defined by at least two waypoints) into a set of Orca waypoints.
    // Non-position parameters are taken from startParams, intermediateParams, and goalParams.
    std::vector<orca::Waypoint2d>
    convert( const orca::Waypoint2d &startParams,
             const orca::Waypoint2d &goalParams,
             const orca::Waypoint2d &intermediateParams,
             const vector<hydrointerfaces::PathPlanner2d::Point> &pathPoints )
    {
        assert( pathPoints.size() > 1 );

        // Compute the length of each leg of the trip
        std::vector<double> legLengths;
        double totalLength=0;
        for ( size_t i=1; i < pathPoints.size(); i++ )
        {
            double thisLength = dist( pathPoints[i], pathPoints[i-1] );
            legLengths.push_back( thisLength );
            totalLength += thisLength;
        }

        double totalTime = goalParams.timeTarget - startParams.timeTarget;
        if ( totalTime < 0 )
        {
            stringstream ss;
            ss << "Time of goal was _before_ time of start!" << endl
               << "  startWp.timeTarget: " << startParams.timeTarget << endl
               << "  goalWp.timeTarget:  " << goalParams.timeTarget;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        // Add each leg to the orca-style path
        orca::Path2d orcaPath;
        for ( size_t i=0; i < pathPoints.size(); i++ )
        {
            orca::Waypoint2d wp;

            // Set the non-position parameters
            if ( i==0 )                        wp = startParams;
            else if ( i==pathPoints.size()-1 ) wp = goalParams;
            else                               wp = intermediateParams;

            wp.target.p.x = pathPoints[i].x;
            wp.target.p.y = pathPoints[i].y;

            if ( i != 0 && i != pathPoints.size()-1 )
            {
                // Set the time
                if ( totalTime == 0 )
                {
                    wp.timeTarget = goalParams.timeTarget;
                }
                else
                {
                    const double fractionOfTotalLength = legLengths[i-1]/totalLength;
                    const double timeThisLeg = fractionOfTotalLength*totalTime;
                    wp.timeTarget = orcaPath.back().timeTarget;
                    wp.timeTarget += timeThisLeg;
                }
            }
            orcaPath.push_back( wp );
        }
        return orcaPath;
    }
}

Driver::Driver( hydrointerfaces::PathPlanner2d &hydroDriver,
                double                          intermediateWaypointDistTolerance,
                const orcaice::Context         &context)
    : hydroDriver_(hydroDriver),
      intermediateWaypointDistTolerance_(intermediateWaypointDistTolerance),
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
    path.clear();

    if ( task.coarsePath.size() == 0 )
    {
        stringstream ss;
        ss << "task.coarsePath.size() was zero.";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    const orca::Path2d *coarsePath = &(task.coarsePath);

    // for each segment in the coarse path
    for (unsigned int i=1; i<coarsePath->size(); i++)
    {
        const orca::Waypoint2d &startWp = (*coarsePath)[i-1];
        const orca::Waypoint2d &goalWp = (*coarsePath)[i];

        //
        // Compute the path segment
        //
        hydroutil::CpuStopwatch stopWatch(true);
        vector<hydrointerfaces::PathPlanner2d::Point> pathSegment = computePathSegment( startWp.target.p.x, 
                                                                                        startWp.target.p.y,
                                                                                        goalWp.target.p.x, 
                                                                                        goalWp.target.p.y );
        stringstream ss;
        ss << "Driver: Computing path segment took " << stopWatch.elapsedSeconds() << "s";
        context_.tracer().debug(ss.str(),1);

        //
        // Convert to orca-style
        //

        // Specify the non-position parameters of intermediate waypoints
        orca::Waypoint2d intermediateWpParams = goalWp;
        intermediateWpParams.target.o = 0;
        intermediateWpParams.headingTolerance = 2*M_PI;
        intermediateWpParams.distanceTolerance = intermediateWaypointDistTolerance_;
            
        std::vector<orca::Waypoint2d> finePathSegment = convert( startWp, 
                                                                 goalWp,
                                                                 intermediateWpParams,
                                                                 pathSegment );

        std::vector<orca::Waypoint2d>::iterator firstWpIt = finePathSegment.begin();
        if ( path.size() != 0 )
        {
            // Ignore the first wp, it was the last wp of the previous segment
            firstWpIt++;
        }
        path.insert( path.end(), firstWpIt, finePathSegment.end() );
    }
}

}
