/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "reflectorextractor.h"
#include <iostream>
#include <assert.h>
#include <cmath>
#include <orca/featuremap2d.h>

using namespace std;

namespace laserfeatures {

void 
ReflectorExtractor::addFeatures( const orca::LaserDataPtr    &laserData,
                                 orca::PolarFeature2dDataPtr &features )
{
    assert( laserMaxRange_ > 0.0 );

    // Are we currently trying to put a cluster together?
    bool buildingTarget = false;

    int    numFeaturePoints             = 0;
    double featureRangeSum              = -1.0;
    double featureBearingSum            = -1.0;
    double maxDeltaRangeWithinReflector = 0.0;

    // Don't iterate over the endpoints, so that the helper-functions
    // don't have to check.
    for ( unsigned int i=1; i<laserData->ranges.size()-1; i++ )
    {
        // Is this a reflector inside range?
        if ( ( laserData->intensities[i] >= minReflectorBrightness_ ) &&
             ( laserData->ranges[i] < laserMaxRange_ ) )
        {
            if ( isSketchy( laserData, i ) )
            {
                // cout<<"TRACE(reflectorextractor.cpp): return " << i << " was bright but sketchy." << endl;

                // Invalidate the current target if we were building one
                buildingTarget = false;
                continue;
            }

            if ( !buildingTarget )
            {
                // cout<<"TRACE(reflectorextractor.cpp): return "<<i
                //     <<": started a target at bearing " << calcBearing(laserData,i)*180.0/M_PI << "deg" << endl;

                // Start building a target
                buildingTarget            = true;
                numFeaturePoints          = 1;
                featureRangeSum           = laserData->ranges[i];
                featureBearingSum         = calcBearing( laserData, i );
                maxDeltaRangeWithinReflector = 0.0;
            }
            else
            {
                // Continue building the target
                numFeaturePoints++;
                featureRangeSum   += laserData->ranges[i];
                featureBearingSum += calcBearing( laserData, i );
                if ( fabs(deltaRange( laserData, i )) > maxDeltaRangeWithinReflector )
                    maxDeltaRangeWithinReflector = deltaRange( laserData, i );
            }
        }
        else
        {
            // This is not a reflective return.
            // Does this mark the end of a cluster?
            if ( buildingTarget )
            {
                // Invalidate the cluster if something's sketchy.
                if ( ( isSketchy( laserData, i ) ) ||
                     ( maxDeltaRangeWithinReflector > maxDeltaRangeWithinReflector_ ) )
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": was building a target, but end was sketchy." << endl;
                    buildingTarget = false;
                }
                else
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": finished a target." << endl;

                    // Create a new feature from the cluster
                    buildingTarget = false;
                    orca::SinglePolarFeature2dPtr f = new orca::SinglePolarFeature2d;
                    f->type = orca::feature::LASERREFLECTOR;
                    f->p.r  = featureRangeSum / numFeaturePoints;
                    f->p.o  = featureBearingSum / numFeaturePoints;
                    features->features.push_back( f );
                }
            }
        }
    }
}

bool 
ReflectorExtractor::isSketchy( const orca::LaserDataPtr &laserData,
                               int returnNum )
{
    assert( returnNum != 0 && returnNum != (int)(laserData->ranges).size()-1 );

    if ( fabs( deltaRange(laserData,returnNum)   ) > maxDeltaRangeNearReflector_ ||
         fabs( deltaRange(laserData,returnNum+1) ) > maxDeltaRangeNearReflector_  )
    {
        return true;
    }
    else
    {
        return false;
    }
}

}
