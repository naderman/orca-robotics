/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#ifndef GPS_HEURISTICS_H
#define GPS_HEURISTICS_H

#include <orca/gps.h>
#include <orcaice/context.h>

namespace gps2localise2d {

    
//
// @brief A bunch of heuristics which are used to discard suspicious GPS frames
// 
// @author Tobias Kaupp
//    
class GpsHeuristics
{

public:
    
    GpsHeuristics( const orcaice::Context &context,
                   const double           &maxSpeed );
    
    // Returns true if there are enough satellites
    bool haveEnoughSatellites( int numSat );
    
    // Returns true if we have a valid fix
    bool haveValidFix( const orca::GpsPositionType &type );
    
    // Checks two things: (1) whether the reported speed is consistent with the speed 
    // calculated based on position diffs, and (2) whether we have 'teleoported' based
    // on the maximum speed the vehicle can do
    // Returns 0 if ok (both checks pass), 1 if not ok (at least one check fails), and 2 if we can't determine
    int checkSpeedAndPosition( const double     &northing,
                               const double     &easting,
                               const double     &speedReported,
                               const orca::Time &timeStamp );
    
private:
    
    orcaice::Context context_;
    
    // one-step memory to compute differentials
    bool firstTime_;
    double lastEasting_;
    double lastNorthing_;
    orca::Time lastTimeStamp_;
    
    // config variables
    double maxSpeed_;
    int minNumSatellites_;
    double maxTimeDiff_;
    double speedDiffFactor_;
    
    void saveData( const double     &northing,
                   const double     &easting,
                   const orca::Time &timeStamp );

};

}

#endif
