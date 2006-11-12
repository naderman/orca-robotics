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
 
#ifndef ORCA2_LASERFEATUREEXTRACTOR_COMBINED_DRIVER_H
#define ORCA2_LASERFEATUREEXTRACTOR_COMBINED_DRIVER_H

#include "algorithmdriver.h"

class ReflectorExtractor;
class ForegroundExtractor;
class DoorExtractor;
class CornerExtractor;

namespace laserfeatures
{

class CombinedDriver: public AlgorithmDriver
{

public:

    struct Config
    {
        // which algs
        int    extractReflectors; 
        int    extractForegroundPoints; 
        int    extractCorners; 
        int    extractDoors; 
        // reflector params
        double maxDeltaRangeNearReflector;
        double maxDeltaRangeWithinReflector;
        int    minReflectorBrightness; 
        // foreground params
        double minForegroundWidth;
        double maxForegroundWidth;
        double minForegroundBackgroundSeparation;
    };
    
    CombinedDriver( const Config & config );
    virtual ~CombinedDriver();
    
    virtual int computeFeatures( const orca::RangeScanner2dConfigPtr &laserConfigPtr,
                                 const orca::LaserScanner2dDataPtr          &laserDataPtr,
                                 orca::PolarFeature2dDataPtr       &featureDataPtr );
    
    virtual void setMaxRange( float maxRange );

private:

    ReflectorExtractor *reflectorExtractor_;
    ForegroundExtractor *foregroundExtractor_;
    CornerExtractor *cornerExtractor_;
    DoorExtractor *doorExtractor_;

    Config config_;

};

} // namespace

#endif
