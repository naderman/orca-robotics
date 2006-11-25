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
#include "reflectorextractor.h"
#include <iostream>
#include <assert.h>
#include <cmath>
#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.05;
    const double P_TRUE_POSITIVE  = 0.75;
}


static double laserScanBearing( const orca::RangeScanner2dDataPtr & scan, const int i )
{
    double angleIncrement = scan->fieldOfView / double(scan->ranges.size()+1);
    return (scan->startAngle + angleIncrement*i);
}

ReflectorExtractor::ReflectorExtractor( orcaice::Context context, double laserMaxRange )
    : laserMaxRange_(laserMaxRange)
{
    std::string prefix = context.tag() + ".Config.Reflectors.";
    Ice::PropertiesPtr prop = context.properties();

    maxDeltaRangeWithinReflector_      =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWithinReflector", 0.3 );
    maxDeltaRangeNearReflector_        =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeNearReflector", 0.5 );
    minReflectorBrightness_            =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"MinBrightness", 1);
}

void 
ReflectorExtractor::addFeatures( const orca::LaserScanner2dDataPtr    &laserData,
                                 orca::PolarFeature2dDataPtr &features )
{
//     cout<<"TRACE(reflectorextractor.cpp): addFeatures()" << endl;

    assert( laserMaxRange_ > 0.0 );

    const int numReturnsToGetOverSketch = 4;

    // Are we currently trying to put a cluster together?
    bool buildingTarget = false;

    int    numFeaturePoints             = 0;
    double featureRangeSum              = -1.0;
    double featureBearingSum            = -1.0;
    double maxDeltaRangeWithinReflector = 0.0;
    int    lastSketchReturn             = -numReturnsToGetOverSketch;

//     cout<<"TRACE(reflectorextractor.cpp): laserData->ranges.size(): " << laserData->ranges.size() << endl;
//     cout<<"TRACE(reflectorextractor.cpp): laserMaxRange_: " << laserMaxRange_ << endl;
//     cout<<"TRACE(reflectorextractor.cpp): minReflectorBrightness: " << minReflectorBrightness_ << endl;

    // Don't iterate over the endpoints, so that the helper-functions
    // don't have to check.
    for ( unsigned int i=1; i<laserData->ranges.size()-1; i++ )
    {
        // cout<<"TRACE(reflectorextractor.cpp): r,i: " << laserData->ranges[i] << "," << (int)(laserData->intensities[i]) << endl;

        // Is this a reflector inside range?
        if ( ( laserData->intensities[i] >= minReflectorBrightness_ ) &&
             ( laserData->ranges[i] < laserMaxRange_ ) )
        {
            // cout<<"TRACE(reflectorextractor.cpp): =========================" << endl;
            if ( (int)i-lastSketchReturn < numReturnsToGetOverSketch )
            {
                // Remain sketched-out while there are still bright things.
                lastSketchReturn = i;
                buildingTarget = false;
                continue;
            }

            if ( !buildingTarget )
            {
                // Start building a target?
                if ( isSketchy( laserData, i, true ) )
                {
                    lastSketchReturn = i;
                    continue;
                }
                else
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i
                    //     <<": started a target at bearing " << laserScanBearing(laserData,i)*180.0/M_PI << "deg" << endl;

                    buildingTarget            = true;
                    numFeaturePoints          = 1;
                    featureRangeSum           = laserData->ranges[i];
                    featureBearingSum         = laserScanBearing( laserData, i );
                    maxDeltaRangeWithinReflector = 0.0;
                }
            }
            else
            {
                if ( isSketchy( laserData, i, false ) )
                {
                    // Invalidate the current target
                    buildingTarget = false;
                    lastSketchReturn = i;
                    continue;
                }
                else
                {
                    // Continue building the target
                    numFeaturePoints++;
                    featureRangeSum   += laserData->ranges[i];
                    featureBearingSum += laserScanBearing( laserData, i );
                    if ( fabs(deltaRange( laserData, i )) > maxDeltaRangeWithinReflector )
                        maxDeltaRangeWithinReflector = deltaRange( laserData, i );
                }
            }
        }
        else
        {
            // This is not a reflective return.
            // Does this mark the end of a cluster?
            if ( buildingTarget )
            {
                // Invalidate the cluster if something's sketchy.
                if ( ( isSketchy( laserData, i, false ) ) ||
                     ( maxDeltaRangeWithinReflector > maxDeltaRangeWithinReflector_ ) )
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": was building a target, but end was sketchy." << endl;
                    lastSketchReturn = i;
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
                    f->pFalsePositive = P_FALSE_POSITIVE;
                    f->pTruePositive = P_TRUE_POSITIVE;
                    features->features.push_back( f );
                }
            }
        }
    }
}

bool 
ReflectorExtractor::isSketchy( const orca::LaserScanner2dDataPtr &laserData,
                               int returnNum,
                               bool reflectorStart )
{
    assert( returnNum != 0 && returnNum != (int)(laserData->ranges).size()-1 );

    bool sketchy=false;
    if ( reflectorStart )
    {
        // Invalidate for big increases in range: ie viewing a reflector behind a
        // foreground obstacle to the right.
        if ( deltaRange(laserData,returnNum) > maxDeltaRangeNearReflector_ )
            sketchy=true;
        if ( deltaRange(laserData,returnNum+1) > maxDeltaRangeNearReflector_ )
            sketchy=true;
    }
    else
    {
        // Invalidate for big drops in range: ie viewing a reflector behind a
        // foreground obstacle to the left.
        if ( deltaRange(laserData,returnNum) < -maxDeltaRangeNearReflector_ )
            sketchy=true;
        if ( deltaRange(laserData,returnNum+1) < -maxDeltaRangeNearReflector_ )
            sketchy=true;
    }
    return sketchy;
}

}
