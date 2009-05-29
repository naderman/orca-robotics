/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <sstream>
#include <iomanip>
#include <orcaice/orcaice.h>
#include "gpsheuristics.h"
#include <cmath>

using namespace std;

namespace gps2localise2d {
    
namespace {
    
    double computeDistanceTravelled( const double &northing, 
                                     const double &easting,
                                     const double &lastNorthing,
                                     const double &lastEasting )
    {
        double dX = easting - lastEasting;
        double dY = northing - lastNorthing;
        return sqrt( dX*dX + dY*dY );
    }   
}
    
GpsHeuristics::GpsHeuristics( const orcaice::Context &context,
                              const double           &maxSpeed ) 
    : context_(context),
      firstTime_(true),
      maxSpeed_(maxSpeed)
      
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    minNumSatellites_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinNumSatellites", 4 );
    maxTimeDiff_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxTimeDifferenceSeconds", 2.5 );
    speedDiffFactor_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"SpeedDifferenceFactor", 2.0 );
}

bool
GpsHeuristics::haveEnoughSatellites( int numSat )
{
    if (numSat < minNumSatellites_)
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

void 
GpsHeuristics::saveData( const double     &northing,
                         const double     &easting,
                         const orca::Time &timeStamp )
{
    lastNorthing_ = northing;
    lastEasting_ = easting;
    lastTimeStamp_ = timeStamp;
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
    
    // compute difference between timestamps
    double timeDiff = orcaice::timeDiffAsDouble( timeStamp, lastTimeStamp_);
    if (timeDiff==0.0) {
        context_.tracer().debug( "Time difference between consecutive timestamps is 0.0", 2 );
        saveData( northing, easting, timeStamp );
        return 2;
    }
    
    // check whether we've received anything recently
    if ( timeDiff > maxTimeDiff_ ) {
        stringstream ss; 
        ss << "Too long since the last time we've received a timestamp: " << fixed << setprecision(1) << timeDiff << "s. Can't compute an estimate for speed or travelled distance.";
        context_.tracer().debug( ss.str(), 2 );
        saveData( northing, easting, timeStamp );
        return 2;
    }
    
    // compute travelled distance since last time
    double distanceTravelled = computeDistanceTravelled( northing, 
                                                         easting, 
                                                         lastNorthing_, 
                                                         lastEasting_ );
    // save for next time
    saveData( northing, easting, timeStamp );
    
    // check for teleport
    if ( distanceTravelled > (maxSpeed_*timeDiff) )
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
    double speedDiff = fabs(speedComputed - speedReported);
    if ( (speedDiff > speedDiffFactor_ * speedReported) && (speedReported!=0.0) )
    {
        stringstream ss;
        ss << "Reported speed (" << fixed << setprecision(2) << speedReported << "m/s) is inconsistent with what we've computed based on positions (" << speedComputed << "m/s)";
        context_.tracer().debug( ss.str(), 2 );
        return 1;
    }
    
    context_.tracer().debug("Speed and position checks have passed. The GPS record seems valid.", 3 );
    return 0;
}
    
}
