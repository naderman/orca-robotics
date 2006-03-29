#ifndef REFLECTOREXTRACTOR__H
#define REFLECTOREXTRACTOR__H

#include <orca/laser.h>
#include <orca/polarfeature2d.h>

namespace laserfeatures {

//!
//! @author Alex Brooks
//!
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
    
    // This function returns true if there's a big jump in the laser
    // ranges near the return in question.
    // The function exists to test for bright returns in weird places
    // as the laser grazes past corners etc.
    bool isSketchy( const orca::LaserDataPtr &laserData,
                    int returnNum );

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
