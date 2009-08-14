/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "reflectorextractor.h"
#include <iostream>
#include <assert.h>
#include <cmath>
#include <sstream>

using namespace std;

namespace hydrolfextract {

bool
ReflectorExtractor::Config::isSane() const
{
    if ( !scannerConfig.isSane()             ||
         maxDeltaRangeNearReflector   <  0 ||
         maxDeltaRangeWithinReflector <  0 ||
         minReflectorBrightness       <= 0 )
    {
        return false;
    }
    return true;
}

std::vector<Reflector> 
ReflectorExtractor::extractReflectors( const std::vector<float>         &ranges,
                                       const std::vector<unsigned char> &intensities ) const
{
    reflectors_.resize(0);

    // Are we currently trying to put a cluster together?
    bool buildingTarget = false;
    double maxDeltaRangeWithinReflector = 0.0;
    Reflector reflector;
    reflector.startI = -1;
    reflector.endI   = -1;

    // Don't iterate over the endpoints, so that the helper-functions
    // don't have to check.
    for ( unsigned int i=1; i<ranges.size()-1; i++ )
    {
        // Is this a reflector inside range?
        if ( ( intensities[i] >= config_.minReflectorBrightness ) &&
             ( isValidRange( ranges[i], config_.scannerConfig ) ) )
        {
            // cout<<"TRACE(reflectorextractor.cpp): =========================" << endl;
            if ( !buildingTarget )
            {
                // Start building a target?
                if ( isSketchy( ranges, i ) )
                {
                    continue;
                }
                else
                {
                    buildingTarget            = true;
                    reflector.startI          = i;
                    maxDeltaRangeWithinReflector = 0.0;
                }
            }
            else
            {
                if ( isSketchy( ranges, i ) )
                {
                    // Invalidate the current target
                    buildingTarget = false;
                    continue;
                }
                else
                {
                    // Continue building the target
                    if ( fabs(deltaRange( ranges, i )) > maxDeltaRangeWithinReflector )
                        maxDeltaRangeWithinReflector = deltaRange( ranges, i );
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
                if ( ( isSketchy( ranges, i ) ) ||
                     ( maxDeltaRangeWithinReflector > config_.maxDeltaRangeWithinReflector ) )
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": was building a target, but end was sketchy." << endl;
                    buildingTarget = false;
                }
                else
                {
                    // cout<<"TRACE(reflectorextractor.cpp): return "<<i<<": finished a target." << endl;

                    // Create a new feature from the cluster
                    buildingTarget = false;
                    reflector.endI = i-1;
                    reflectors_.push_back( reflector );
                }
            }
        }
    }

    return reflectors_;
}

bool 
ReflectorExtractor::isSketchy( const std::vector<float> &ranges,
                               int returnNum ) const
{
    assert( returnNum != 0 && returnNum != (int)(ranges).size()-1 );

    return ( fabs(deltaRange(ranges,returnNum)) > config_.maxDeltaRangeNearReflector );
}

////////////////////////////////////////////////////////////////////////////////

double
calcReflectorPFalsePositive( const std::vector<float> &ranges,
                             int start,
                             int end,
                             double bestPossiblePFalsePositive,
                             double maxDeltaRangeNearReflector )
{
    const int numEitherSide=2;
    if ( start < numEitherSide ) return 1.0;
    if ( (int)(ranges.size())-end < numEitherSide ) return 1.0;

    double pFalse = bestPossiblePFalsePositive;
    double delta;

    for ( int i=0; i < numEitherSide; i++ )
    {
        delta = fabs(ranges[start-i]-ranges[start-i-1]);
        if ( delta > maxDeltaRangeNearReflector )
        {
            if ( i==0 )
                return 1.0;
            else
                pFalse *= 2;
        }

        delta = fabs(ranges[end+i]-ranges[end+i+1]);
        if ( delta > maxDeltaRangeNearReflector )
        {
            if ( i==0 )
                return 1.0;
            else
                pFalse *= 2;
        }
    }
    return pFalse;
}

geom2d::PolarPoint
calcObsRB( const std::vector<float>           &ranges,
           const hydroscanutil::ScannerConfig &scannerConfig,
           const Reflector                    &reflector )
{
    double rangeSum=0;
    double bearingSum=0;
    for ( int rangeI=reflector.startI; rangeI <= reflector.endI; rangeI++ )
    {
        rangeSum   += ranges[rangeI];
        bearingSum += scanBearing( scannerConfig, rangeI );
    }
    const double range = rangeSum/(double)(reflector.numReturns());
    const double bearing = bearingSum/(double)(reflector.numReturns());    
    return geom2d::PolarPoint( range, bearing );
}

////////////////////////////////////////////////////////////////////////////////

std::vector<hydrofeatureobs::PointFeatureObs*> 
ReflectorFeatureExtractor::extractFeatures( const std::vector<float>     &ranges,
                                            const std::vector<Reflector> &reflectors ) const
{
//     // Get the raw reflector information
//     std::vector<hydrolfextract::ReflectorExtractor::Reflector> reflectors =
//         reflectorExtractor_.extractReflectors( ranges, intensities );

    std::vector<hydrofeatureobs::PointFeatureObs*> featureObservations;
    for ( size_t i=0; i < reflectors.size(); i++ )
    {
        double pFalse = hydrolfextract::calcReflectorPFalsePositive( ranges,
                                                                     reflectors[i].startI,
                                                                     reflectors[i].endI,
                                                                     pFalsePositive_,
                                                                     config_.maxDeltaRangeNearReflector );
        if ( pFalse >= pTruePositive_ )
            continue;

        geom2d::PolarPoint rb = calcObsRB( ranges, config_.scannerConfig, reflectors[i] );

        featureObservations.push_back( new hydrofeatureobs::PointFeatureObs( hydrofeatureobs::LASERREFLECTOR,
                                                                             pFalse,
                                                                             pTruePositive_,
                                                                             rb.range(),
                                                                             rb.bearing(),
                                                                             rangeSd_,
                                                                             bearingSd_ ) );
    }
    return featureObservations;
}

std::string
toString( const Reflector &r )
{
    stringstream ss;
    ss << "startI=" << r.startI << ", endI=" << r.endI;
    return ss.str();
}

}
