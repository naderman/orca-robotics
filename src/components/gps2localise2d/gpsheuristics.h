/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef GPS_HEURISTICS_H
#define GPS_HEURISTICS_H

#include <orca/gps.h>
#include <orcaice/orcaice.h>

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
                   const double           &maxForwardSpeed );
    
    // returns true if there are enough satellites
    bool haveEnoughSatellites( int numSat );
    
    // returns true if we have a valid fix
    bool haveValidFix( const orca::GpsPositionType &type );
    
    // Checks two things: (1) whether the reported speed is consistent with the speed 
    // calculated based on position diffs, and (2) whether we have 'teleoported' based
    // on the maximum speed the vehicle can do
    // Returns 0 if consistent, 1 if inconsistent, and 2 if we can't determine
    int checkSpeedAndPosition( const double     &northing,
                               const double     &easting,
                               const double     &speedReported,
                               const orca::Time &timeStamp );
    
private:
    
    orcaice::Context context_;
    
    double maxForwardSpeed_;
    bool firstTime_;
    double lastEasting_;
    double lastNorthing_;
    orca::Time lastTimeStamp_;
    
    void saveData( const double     &northing,
                   const double     &easting,
                   const orca::Time &timeStamp );
    
    double computeDistanceTravelled( const double &northing, 
                                     const double &easting );

};

}

#endif
