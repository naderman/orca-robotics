/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "doorextractor.h"
#include <iostream>
#include <cmath>
#include <hydroscanutil/scanutil.h>
#include <sstream>

using namespace std;

namespace hydrolfextract {

DoorJamb::DoorJamb( const hydroscanutil::ScannerConfig &scannerConfig,
                    const std::vector<float>           &ranges,
                    int                                 pReturnI )
    : returnI(pReturnI),
      range(ranges[pReturnI]),
      bearing(scanBearing(scannerConfig,pReturnI)),
      x(range*cos(bearing)),
      y(range*sin(bearing))
{}

std::string toString( const Door &d )
{
    stringstream ss;
    ss << "start="<<d.start<<", end="<<d.end;
    return ss.str();
}

//////////////////////////////////////////////////////////////////////

bool 
DoorExtractor::Config::isSane() const
{
    if ( !scannerConfig.isSane() ||
         minDoorWidth < 0 ||
         maxDoorWidth < 0 ||
         maxDoorWidth < minDoorWidth ||
         minDeltaRangeThruDoor < 0 ||
         minReturnsThruDoor < 1 ||
         minReturnsBesideDoor < 0 ||
         maxDeltaRangeWallBesideDoor < 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

//////////////////////////////////////////////////////////////////////

DoorExtractor::DoorExtractor( const Config &config, const hydroutil::Context &context )
    : config_(config),
      context_(context)
{
    assert( config_.isSane() );

    minDoorWidthSq_ = config_.minDoorWidth*config_.minDoorWidth;
    maxDoorWidthSq_ = config_.maxDoorWidth*config_.maxDoorWidth;
}

void
DoorExtractor::findPossibleDoorJambs( const std::vector<float> &ranges,
                                      std::vector<DoorJamb>    &possibleStarts,
                                      std::vector<DoorJamb>    &possibleEnds ) const
{
    //
    // Look for discontinuities
    //
    for ( unsigned int i=1+config_.minReturnsBesideDoor;
          i < ranges.size()-1-config_.minReturnsBesideDoor;
          i++ )
    {
        double deltaRange = ranges[i] - ranges[i-1];
        if ( deltaRange > config_.minDeltaRangeThruDoor )
        {
            // an increase in range: a possible start...
            // Step from back a bit up to the possible start, making sure the wall's OK.
            bool wallNextToDoorOK = true;
            for ( unsigned int j=i-1-config_.minReturnsBesideDoor; j < i-1; j++ )
            {
                if ( fabs(ranges[j]-ranges[j+1]) > config_.maxDeltaRangeWallBesideDoor )
                {
                    wallNextToDoorOK = false;
                    break;
                }
            }
            if ( wallNextToDoorOK )
                possibleStarts.push_back( DoorJamb( config_.scannerConfig, ranges, i-1 ) );
        }
        else if ( deltaRange < -config_.minDeltaRangeThruDoor )
        {
            // a decrease in range: a possible end...
            // Step forwards from the end, making sure the wall's OK.
            bool wallNextToDoorOK = true;
            for ( unsigned int j=i; j < i+config_.minReturnsBesideDoor; j++ )
            {
                if ( fabs(ranges[j]-ranges[j+1]) > config_.maxDeltaRangeWallBesideDoor )
                {
                    wallNextToDoorOK = false;
                    break;
                }
            }
            if ( wallNextToDoorOK )
                possibleEnds.push_back( DoorJamb( config_.scannerConfig, ranges, i ) );
        }
    }
}

void
DoorExtractor::findActualDoors( const std::vector<float> &ranges,
                                const std::vector<DoorJamb> &possibleDoorStarts,
                                const std::vector<DoorJamb> &possibleDoorEnds,
                                std::vector<Door> &actualDoors ) const
{
    // cout<<"TRACE(doorextractor.cpp): "<<__func__ << endl;

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
            if ( (end.returnI - start.returnI) < (config_.minReturnsThruDoor+1) )
            {
                // cout<<"TRACE(doorextractor.cpp): can only see " << (end.returnI - start.returnI) << " returns thru door." << endl;
                continue;
            }

            // Gotta be of about the right width
            double apparentWidthSq = (start.y-end.y)*(start.y-end.y) + (start.x-end.x)*(start.x-end.x);
            if ( ! (apparentWidthSq >= minDoorWidthSq_ &&
                    apparentWidthSq <= maxDoorWidthSq_ ) )
            {
                // cout<<"  TRACE(doorextractor.cpp): Door not of right width: w="<<sqrt(apparentWidthSq) << endl;
                continue;
            }

            // Should be an increase in depth through the doorway
            bool somethingInDoorway=false;
            for ( int j=start.returnI+1; j < end.returnI; j++ )
            {
                if ( ranges[j] < ((start.range+end.range)/2.0 + config_.minDeltaRangeThruDoor ) )
                {
                    // cout<<"TRACE(doorextractor.cpp): something's in (or just behind) the doorway" << endl;
                    somethingInDoorway=true;
                    break;
                }
            }
            if ( somethingInDoorway ) continue;

            // Everything checks out.
            // cout<<"  TRACE(doorextractor.cpp): Valid door found!" << endl;
            actualDoors.push_back( Door(start,end) );
        }
    }
}

std::vector<Door> 
DoorExtractor::extractDoors( const std::vector<float> &ranges ) const
{
    context_.tracer().debug( " --- DoorExtractor::extractDoors() ---", 2 );

    std::vector<hydrofeatureobs::PointFeatureObs> features;

    std::vector<DoorJamb> possibleDoorStarts, possibleDoorEnds;
    findPossibleDoorJambs( ranges,
                           possibleDoorStarts,
                           possibleDoorEnds );

    if ( context_.tracer().verbosity( gbxutilacfr::DebugTrace, gbxutilacfr::ToAny ) >= 2 )
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
        context_.tracer().debug( ss.str(), 2 );
    }

    std::vector<Door> actualDoors;
    findActualDoors( ranges, possibleDoorStarts, possibleDoorEnds, actualDoors );

    if ( context_.tracer().verbosity( gbxutilacfr::DebugTrace, gbxutilacfr::ToAny ) >= 2 )
    {
        stringstream ss;
        ss << "Actual doors:" << endl;
        for ( unsigned int i=0; i < actualDoors.size(); i++ )
        {
            ss << "  " << actualDoors[i] << endl;
        }
        context_.tracer().debug( ss.str(), 2 );
    }

    return actualDoors;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<hydrofeatureobs::PointFeatureObs*> 
DoorFeatureExtractor::extractFeatures( const std::vector<float> &ranges,
                                       const std::vector<Door>  &doors ) const
{
    std::vector<hydrofeatureobs::PointFeatureObs*> features;
    for ( size_t i=0; i < doors.size(); i++ )
    {
        // Pick the mid-point
        const double x = (doors[i].start.x+doors[i].end.x)/2.0;
        const double y = (doors[i].start.y+doors[i].end.y)/2.0;
        const double range = hypot(y,x);
        const double bearing = atan2(y,x);

        features.push_back( new hydrofeatureobs::PointFeatureObs( hydrofeatureobs::DOOR,
                                                                  pFalsePositive_,
                                                                  pTruePositive_,
                                                                  range,
                                                                  bearing,
                                                                  rangeSd_,
                                                                  bearingSd_ ) );
    }
    return features;
}

}
