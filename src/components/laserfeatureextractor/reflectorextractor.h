/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef REFLECTOREXTRACTOR__H
#define REFLECTOREXTRACTOR__H

#include <orca/laser.h>
#include <orca/polarfeature2d.h>

namespace laserfeatures {

class ReflectorExtractor
{

public: 

    ReflectorExtractor( double maxDeltaRangeNearReflector,
                        double maxDeltaRangeWithinReflector,
                        double minReflectorBrightness )
        : maxDeltaRangeNearReflector_(maxDeltaRangeNearReflector),
          maxDeltaRangeWithinReflector_(maxDeltaRangeWithinReflector),
          minReflectorBrightness_(minReflectorBrightness),
          laserMaxRange_( -1 )
        {}

    // Adds laser features to the 'features' data structure
    void addFeatures( const orca::LaserDataPtr    &laserData,
                      orca::PolarFeature2dDataPtr &features );

    void setMaxRange( double maxRange ) { laserMaxRange_ = maxRange; }

private: 

    double maxDeltaRangeNearReflector_;
    double maxDeltaRangeWithinReflector_;
    double minReflectorBrightness_;
    double laserMaxRange_;

    // 
    // Helper functions.  These assume that you're not asking for
    // info about the first or last return
    //

    // This function exists to test for bright returns in weird places
    // as the laser grazes past corners etc.
    // It returns true for reflectors that are partially-obscured by foreground
    // objects.
    bool isSketchy( const orca::LaserDataPtr &laserData,
                    int returnNum,
                    bool reflectorStart );

    // Returns the change in range between this return and the previous one.
    double deltaRange( const orca::LaserDataPtr &laserData,
                       int returnNum )
        { 
            assert( returnNum != 0 );
            return laserData->ranges[returnNum] - laserData->ranges[returnNum-1]; 
        }

    double calcBearing( const orca::LaserDataPtr &laserData,
                        int returnNum )
        { return laserData->startAngle + returnNum*laserData->angleIncrement; }
};

}

#endif
