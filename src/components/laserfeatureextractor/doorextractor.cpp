/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "doorextractor.h"
#include <orcaice/orcaice.h>
#include <iostream>
#include <cmath>
#include "scanutil.h"
#include <orca/featuremap2d.h>

using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.3;
    const double P_TRUE_POSITIVE  = 0.6;
}

DoorJamb::DoorJamb( const orca::LaserScanner2dDataPtr &data, int i )
    : returnI(i),
      range( data->ranges[i] ),
      bearing(laserScanBearing( data, i )),
      x(range*cos(bearing)),
      y(range*sin(bearing))
{}

std::ostream &operator<<( std::ostream &s, const DoorJamb &d )
{
    return s << "[i,r,b = "<<d.returnI<<", "<<d.range<<", "<<d.bearing*180.0/M_PI<<"]";
}

DoorExtractor::DoorExtractor( const orcaice::Context &context )
    : context_(context)
{
    std::string prefix = context.tag() + ".Config.Doors.";
    Ice::PropertiesPtr prop = context.properties();

    double minDoorWidth = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinDoorWidth", 0.9 );
    double maxDoorWidth = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDoorWidth", 1.2 );
    
    minDoorWidthSq_ = minDoorWidth*minDoorWidth;
    maxDoorWidthSq_ = maxDoorWidth*maxDoorWidth;
    minDeltaRangeThruDoor_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinDeltaRangeThruDoor", 2.0 );

    rangeSd_   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"RangeSd", 0.3 );
    bearingSd_ = M_PI/180.0*orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"BearingSd", 5.0 );

    minReturnsThruDoor_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinReturnsThruDoor", 3 );
    minReturnsBesideDoor_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinReturnsBesideDoor", 2 );

    maxDeltaRangeWallBesideDoor_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWallBesideDoor", 0.3 );
}

void
DoorExtractor::findPossibleDoorJambs( const orca::LaserScanner2dDataPtr &data,
                                      std::vector<DoorJamb> &possibleStarts,
                                      std::vector<DoorJamb> &possibleEnds )
{
    //
    // Look for discontinuities
    //

    for ( unsigned int i=1+minReturnsBesideDoor_; i < data->ranges.size()-1-minReturnsBesideDoor_; i++ )
    {
        double deltaRange = data->ranges[i] - data->ranges[i-1];
        if ( deltaRange > minDeltaRangeThruDoor_ )
        {
            // an increase in range: a possible start...
            // Step from back a bit up to the possible start, making sure the wall's OK.
            bool wallNextToDoorOK = true;
            for ( unsigned int j=i-1-minReturnsBesideDoor_; j < i-1; j++ )
            {
                if ( fabs(data->ranges[j]-data->ranges[j+1]) > maxDeltaRangeWallBesideDoor_ )
                {
                    wallNextToDoorOK = false;
                    break;
                }
            }
            if ( wallNextToDoorOK )
                possibleStarts.push_back( DoorJamb( data, i-1 ) );
        }
        else if ( deltaRange < -minDeltaRangeThruDoor_ )
        {
            // a decrease in range: a possible end...
            // Step forwards from the end, making sure the wall's OK.
            bool wallNextToDoorOK = true;
            for ( unsigned int j=i; j < i+minReturnsBesideDoor_; j++ )
            {
                if ( fabs(data->ranges[j]-data->ranges[j+1]) > maxDeltaRangeWallBesideDoor_ )
                {
                    wallNextToDoorOK = false;
                    break;
                }
            }
            if ( wallNextToDoorOK )
                possibleEnds.push_back( DoorJamb( data, i ) );
        }
    }
}

void
DoorExtractor::findActualDoorJambs( const std::vector<DoorJamb> &possibleDoorStarts,
                                    const std::vector<DoorJamb> &possibleDoorEnds,
                                    std::vector<DoorJamb> &actualDoorStarts,
                                    std::vector<DoorJamb> &actualDoorEnds )
{
    //
    // Look for start-end combinations the right distance apart.
    //

    // For each door start
    for ( unsigned int i=0; i < possibleDoorStarts.size(); i++ )
    {
        const DoorJamb &start = possibleDoorStarts[i];

        // Try to match it with a door end
        for ( unsigned int j=0; j < possibleDoorEnds.size(); j++ )
        {
            const DoorJamb &end = possibleDoorEnds[j];

            // cout<<"TRACE(doorextractor.cpp): start,end: " << start << ", " << end << endl;

            // Start has to be before end
            if ( !( end.bearing > start.bearing ) )
            {
                // cout<<"  TRACE(doorextractor.cpp): start not before end" << endl;
                continue;
            }

            // Need to see a few returns through the door
            if ( (end.returnI - start.returnI) < (minReturnsThruDoor_+1) )
            {
                // cout<<"  TRACE(doorextractor.cpp): Can't see enough thru door." << endl;
                continue;
            }

            double apparentWidthSq = (start.y-end.y)*(start.y-end.y) + (start.x-end.x)*(start.x-end.x);

            if ( ! (apparentWidthSq >= minDoorWidthSq_ &&
                    apparentWidthSq <= maxDoorWidthSq_ ) )
            {
                // cout<<"  TRACE(doorextractor.cpp): Door not of right width." << endl;
                continue;
            }

            // Everything checks out.
            // cout<<"  TRACE(doorextractor.cpp): Valid door found!" << endl;
            actualDoorStarts.push_back( start );
            actualDoorEnds.push_back( end );
        }
    }
}

void DoorExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                 orca::PolarFeature2dDataPtr &features )
{
    context_.tracer()->debug( " --- DoorExtractor::addFeatures() ---", 2 );

    std::vector<DoorJamb> possibleDoorStarts, possibleDoorEnds;
    findPossibleDoorJambs( laserData,
                           possibleDoorStarts,
                           possibleDoorEnds );

    if ( context_.tracer()->verbosity( orcaiceutil::Tracer::DebugTrace, orcaiceutil::Tracer::ToAny ) >= 2 )
    {
        stringstream ss;
        ss << "Possible door starts:" << endl;
        for ( unsigned int i=0; i < possibleDoorStarts.size(); i++ )
        {
            ss << "  " << possibleDoorStarts[i] << endl;
        }
        ss << "Possible door ends: " << endl;
        for ( unsigned int i=0; i < possibleDoorEnds.size(); i++ )
        {
            ss << "  " << possibleDoorEnds[i] << endl;
        }
        context_.tracer()->debug( ss.str(), 2 );
    }

    std::vector<DoorJamb> actualDoorStarts, actualDoorEnds;
    findActualDoorJambs( possibleDoorStarts, possibleDoorEnds, actualDoorStarts, actualDoorEnds );

    if ( context_.tracer()->verbosity( orcaiceutil::Tracer::DebugTrace, orcaiceutil::Tracer::ToAny ) >= 2 )
    {
        stringstream ss;
        ss << "Actual door starts:" << endl;
        for ( unsigned int i=0; i < actualDoorStarts.size(); i++ )
        {
            ss << "  " << actualDoorStarts[i] << endl;
        }
        ss << "Actual door ends: " << endl;
        for ( unsigned int i=0; i < actualDoorEnds.size(); i++ )
        {
            ss << "  " << actualDoorEnds[i] << endl;
        }
        context_.tracer()->debug( ss.str(), 2 );
    }

    assert( actualDoorStarts.size() == actualDoorEnds.size() );
    for ( unsigned int i=0; i < actualDoorStarts.size(); i++ )
    {
        // Pick the mid-point
        orca::PointPolarFeature2dPtr p = new orca::PointPolarFeature2d;
        p->type = orca::feature::DOOR;
        double x = (actualDoorStarts[i].x+actualDoorEnds[i].x)/2.0;
        double y = (actualDoorStarts[i].y+actualDoorEnds[i].y)/2.0;
        p->p.r = hypotf(y,x);
        p->p.o = atan2(y,x);
        p->pFalsePositive = P_FALSE_POSITIVE;
        p->pTruePositive  = P_TRUE_POSITIVE;
        p->rangeSd = rangeSd_;
        p->bearingSd = bearingSd_;
        features->features.push_back( p );
    }

    if ( context_.tracer()->verbosity( orcaiceutil::Tracer::DebugTrace, orcaiceutil::Tracer::ToAny ) >= 2 )
    {
        std::stringstream ss;
        ss << "DoorExtractor:: features: " << orcaice::toString( features );
        context_.tracer()->debug( ss.str() );
    }
}

// void DoorExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
//                                  orca::PolarFeature2dDataPtr &features )
// {    
//     context_.tracer()->debug( " --- DoorExtractor::addFeatures() --- ", 2 );

//     unsigned int i;
//     bool buildingTarget = false;

//     int featureNumPnts = 0;

//     double delta_range = 0;
//     double door_width_sq;
//     double startBearing = - M_PI/2;
//     double startRange = -1.0;
//     double stopBearing = - M_PI/2;
//     double stopRange = -1.0;
//     double startX = 0.0;
//     double startY =0.0;
//     double stopX, stopY;
//     double min_width = 0.9*0.9;
//     double max_width = 1.5*1.5;

//     //std::cout << "Looking for open doors..." << std::endl;
//     // for each return
//     for ( i=10; i<laserData->ranges.size()-10; i++ )
//     {
//         // compute the step change in range
//         delta_range = laserData->ranges[i] - laserData->ranges[i-1];

//         if (buildingTarget == true)
//         {
//             // if we have reached the end of a cluster, process a new observation
//             if ( delta_range < -MIN_DELTA_RANGE_BEHIND_DOOR )
//             {
//                 buildingTarget = false;
//                 stopRange = laserData->ranges[i];
//                 stopBearing = M_PI*(i)/(laserData->ranges.size()-1) - M_PI/2;
//                 stopX = stopRange*cos(stopBearing);
//                 stopY = stopRange*sin(stopBearing);

//                 door_width_sq = (stopX-startX)*(stopX-startX) + (stopY-startY)*(stopY-startY);

//                 //std::cout << " Checking a new cluster at range " << featureRange / featureNumPnts << " bearing " << featureBearing / featureNumPnts << " of width " << pole_width << " with " << featureNumPnts << " points" << std::endl;
//                 // again make sure that the feature is not a foreground point
//                 if (featureNumPnts > 5 && door_width_sq > min_width &&
//                     door_width_sq < max_width )
//                 {
//                     orca::PointPolarFeature2dPtr pp1 = new orca::PointPolarFeature2d;
//                     pp1->type = orca::feature::DOOR;
//                     pp1->p.r = startRange;
//                     pp1->p.o = startBearing;
//                     pp1->pFalsePositive = P_FALSE_POSITIVE;
//                     pp1->pTruePositive  = P_TRUE_POSITIVE;
//                     pp1->rangeSd = RANGE_SD;
//                     pp1->bearingSd = BEARING_SD;

//                     orca::PointPolarFeature2dPtr pp2 = new orca::PointPolarFeature2d;
//                     pp2->type = orca::feature::DOOR;
//                     pp2->p.r = stopRange;
//                     pp2->p.o = stopBearing;
//                     pp2->pFalsePositive = P_FALSE_POSITIVE;
//                     pp2->pTruePositive  = P_TRUE_POSITIVE;
//                     pp2->rangeSd = RANGE_SD;
//                     pp2->bearingSd = BEARING_SD;

//                     features->features.push_back(pp1);
//                     features->features.push_back(pp2);
//                 }

//                 buildingTarget = false;
//             } else {
//                 featureNumPnts++;
//             }  // end of end-of-cluster if
//         }
//         if ( delta_range > MIN_DELTA_RANGE_BEHIND_DOOR )
//         {
//             // start building a new door shape
// //             if ( buildingTarget == false)
// //             {
//                 buildingTarget = true;
//                 featureNumPnts = 1;
//                 startRange = laserData->ranges[i-1];
//                 startBearing = M_PI*(i-1)/(laserData->ranges.size()-1) - M_PI/2;
//                 startX = startRange*cos(startBearing);
//                 startY = startRange*sin(startBearing);
//                 stringstream ss; ss << "DoorExtractor: starting new target at r,b = " << startRange << "," << startBearing*180.0/M_PI;
//                 context_.tracer()->debug( ss.str(), 2 );
// //             }
//         }
//     } // end for each return                                 
// }

}
