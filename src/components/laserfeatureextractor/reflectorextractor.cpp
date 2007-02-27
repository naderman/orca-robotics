/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
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
#include "scanutil.h"

using namespace std;

namespace laserfeatures {

ReflectorExtractor::ReflectorExtractor( const orcaice::Context & context, double laserMaxRange, int numReturns )
    : laserMaxRange_(laserMaxRange)
{
    std::string prefix = context.tag() + ".Config.Reflectors.";
    Ice::PropertiesPtr prop = context.properties();

    maxDeltaRangeWithinReflector_      =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWithinReflector", 0.2 );
    maxDeltaRangeNearReflector_        =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeNearReflector", 0.3 );
    minReflectorBrightness_            =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"MinBrightness", 1);

    pFalsePositive_           =
        orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PFalsePositive", 0.1);
    pTruePositive_            =
        orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PTruePositive", 0.6);

    prefix = context.tag() + ".Config.";
    prop = context.properties();
    rangeSd_   = orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetRangeSd", 0.2);
    bearingSd_ = (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetBearingSd", 5.0);
}

double
ReflectorExtractor::calcPFalsePositive( const orca::LaserScanner2dDataPtr &laserData,
                                        int start,
                                        int end )
{
    const int numEitherSide=2;
    if ( start < numEitherSide ) return -1;
    if ( (int)(laserData->ranges.size())-end < numEitherSide ) return -1;


    double pFalse = pFalsePositive_;
    double delta;

    for ( int i=0; i < numEitherSide; i++ )
    {
        delta = fabs(laserData->ranges[start-i]-laserData->ranges[start-i-1]);
        if ( delta > maxDeltaRangeNearReflector_ )
        {
            if ( i==0 )
                return -1;
            else
                pFalse *= 2;
        }

        delta = fabs(laserData->ranges[end+i]-laserData->ranges[end+i+1]);
        if ( delta > maxDeltaRangeNearReflector_ )
        {
            if ( i==0 )
                return -1;
            else
                pFalse *= 2;
        }
    }
    return pFalse;
}

void 
ReflectorExtractor::addFeatures( const orca::LaserScanner2dDataPtr    &laserData,
                                 orca::PolarFeature2dDataPtr &features )
{
//     cout<<"TRACE(reflectorextractor.cpp): addFeatures()" << endl;
    assert( laserData != 0 );
    assert( laserMaxRange_ > 0.0 );

    // Are we currently trying to put a cluster together?
    bool buildingTarget = false;

    int    numFeaturePoints             = 0;
    double featureRangeSum              = -1.0;
    double featureBearingSum            = -1.0;
    double maxDeltaRangeWithinReflector = 0.0;
    int    targetStart = -1;

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
            if ( !buildingTarget )
            {
                // Start building a target?
                if ( isSketchy( laserData, i ) )
                {
                    continue;
                }
                else
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i
                    //     <<": started a target at bearing " << laserScanBearing(laserData,i)*180.0/M_PI << "deg" << endl;

                    buildingTarget            = true;
                    targetStart               = i;
                    numFeaturePoints          = 1;
                    featureRangeSum           = laserData->ranges[i];
                    featureBearingSum         = laserScanBearing( laserData, i );
                    maxDeltaRangeWithinReflector = 0.0;
                }
            }
            else
            {
                if ( isSketchy( laserData, i ) )
                {
                    // Invalidate the current target
                    buildingTarget = false;
                    targetStart = -1;
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
                if ( ( isSketchy( laserData, i ) ) ||
                     ( maxDeltaRangeWithinReflector > maxDeltaRangeWithinReflector_ ) )
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": was building a target, but end was sketchy." << endl;
                    buildingTarget = false;
                    targetStart = -1;
                }
                else
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": finished a target." << endl;

                    // Create a new feature from the cluster
                    buildingTarget = false;

                    double pFalse = calcPFalsePositive( laserData, targetStart, targetStart+numFeaturePoints );
                    if ( pFalse > 0.0 && pFalse < pTruePositive_ )
                    {
                        orca::PointPolarFeature2dPtr f = new orca::PointPolarFeature2d;
                        f->type = orca::feature::LASERREFLECTOR;
                        f->p.r  = featureRangeSum / numFeaturePoints;
                        f->p.o  = featureBearingSum / numFeaturePoints;
                        f->pFalsePositive = pFalse;
                        f->pTruePositive = pTruePositive_;
                        f->rangeSd = rangeSd_;
                        f->bearingSd = bearingSd_;
                        features->features.push_back( f );
                    }
                }
            }
        }
    }
}

bool 
ReflectorExtractor::isSketchy( const orca::LaserScanner2dDataPtr &laserData,
                               int returnNum )
{
    assert( returnNum != 0 && returnNum != (int)(laserData->ranges).size()-1 );

    return ( fabs(deltaRange(laserData,returnNum)) > maxDeltaRangeNearReflector_ );
}

}
