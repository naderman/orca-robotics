/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <iomanip>
#include <orcaice/timeutils.h>
#include "gpsheuristics.h"

using namespace std;

namespace gps2localise2d {
    
const int MIN_NUM_SATELLITES = 4;
const double MAX_TIME_DIFF = 1.5;
const double SPEED_DIFF_THRESHOLD = 2.0;
    
GpsHeuristics::GpsHeuristics( const orcaice::Context &context,
                              const double           &maxForwardSpeed ) 
    : context_(context),
      maxForwardSpeed_(maxForwardSpeed),
      firstTime_(true)
{
}

bool
GpsHeuristics::haveEnoughSatellites( int numSat )
{
    if (numSat < MIN_NUM_SATELLITES)
    {
        stringstream ss;
        ss << "Not enough satellites: " << numSat << ". Fix not reliable";
        context_.tracer().debug(ss.str(),2);
        return false;
    }
    return true;
}

bool
GpsHeuristics::haveValidFix( const orca::GpsPositionType &type )
{
    if ( type == orca::GpsPositionTypeNotAvailable )
    {
        context_.tracer().debug("No GPS fix!",2);
        return false;
    }
    return true;
}

void GpsHeuristics::saveData( const double     &northing,
                              const double     &easting,
                              const orca::Time &timeStamp )
{
    lastNorthing_ = northing;
    lastEasting_ = easting;
    lastTimeStamp_ = timeStamp;
}

double 
GpsHeuristics::computeDistanceTravelled( const double &northing, 
                                         const double &easting )
{
    double dX = easting - lastEasting_;
    double dY = northing - lastNorthing_;
    return sqrt( dX*dX + dY*dY );
}
    
int
GpsHeuristics::checkSpeedAndPosition( const double     &northing,
                                      const double     &easting,
                                      const double     &speedReported,
                                      const orca::Time &timeStamp )
{
    if (firstTime_) {
        firstTime_ = false;
        context_.tracer().debug( "First record, can't check speed or distance travelled yet", 2 );
        saveData( northing, easting, timeStamp );
        return 2;
    }
    
    // check whether we've received anything recently
    double timeDiff = orcaice::timeDiffAsDouble( timeStamp, lastTimeStamp_);
    if ( timeDiff > MAX_TIME_DIFF ) {
        stringstream ss; 
        ss << "Too long since the last time we've received a timestamp: " << fixed << setprecision(1) << timeDiff << "s. Can't compute an estimate for speed or travelled distance.";
        context_.tracer().debug( ss.str(), 2 );
        saveData( northing, easting, timeStamp );
        return 2;
    }
    
    // compute travelled distance since last time
    double distanceTravelled = computeDistanceTravelled( northing, easting );
    
    // save for next time
    saveData( northing, easting, timeStamp );;
    
    // check for teleport
    if ( distanceTravelled > (maxForwardSpeed_*timeDiff) )
    {
        stringstream ss;
        ss << "It looks like we've teleported, travelled " << fixed << setprecision(1) << distanceTravelled << "m in " << timeDiff << "s";
        context_.tracer().debug( ss.str(), 2 );
        return 1;
    }
    
    // compute speed
    assert(timeDiff!=0.0 && "Time difference was zero");
    double speedComputed = distanceTravelled/timeDiff;
            
    // check for speed consistency
    double speedDiff = abs(speedComputed - speedReported);
    if ( (speedDiff > SPEED_DIFF_THRESHOLD) && (speedReported!=0.0) )
    {
        stringstream ss;
        ss << "Reported speed (" << speedReported << "m/s) is inconsistent with what we've computed based on positions (" << speedComputed << "m/s)";
        context_.tracer().debug( ss.str(), 2 );
        return 1;
    }
    
    context_.tracer().debug("Speed and position checks successfully passed. The GPS record seems valid", 2 );
    return 0;
}
    
}
