/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <assert.h>
#include <cmath>
#include "foregroundextractor.h"
#include <gbxutilacfr/mathdefs.h>
#include <sstream>

using namespace std;

namespace hydrolfextract {

namespace {

    double dist( int i, int j, const std::vector<float> &ranges, double angleIncrement )
    {
        assert( i >= 0 && i < (int)(ranges.size()) );
        assert( j >= 0 && j < (int)(ranges.size()) );

        double xi = ranges[i];
        double yi = 0;
        double xj = ranges[j]*cos((j-i)*angleIncrement);
        double yj = ranges[j]*sin((j-i)*angleIncrement);

        const double distance = hypot( yi-yj, xi-xj );
        return distance;
    }

    double distToClutter( const Pole &pole, 
                          const std::vector<float> &ranges,
                          double startAngle,
                          double angleIncrement,
                          double searchRadius )
    {
        // If we're close to the pole, maybe there's clutter just behind us
        double minDist = pole.range;

        // Check an arc of minDistToClutter either side
        // (straight-line distance is probably better, but it's close enough)
        double angleEitherSide = searchRadius / pole.range;
        int numReturnsEitherSide = (int)(ceil(angleEitherSide / angleIncrement));

        // cout<<"TRACE(foregroundextractor.cpp): pole.range: " << pole.range << ", numReturnsEitherSide: " << numReturnsEitherSide << endl;

        // If the pole's too close to the edge of the scan, maybe
        // there's clutter just outside our field of view
        if ( pole.startI < numReturnsEitherSide )
        {
            minDist = MIN( minDist, pole.range * (pole.startI*angleIncrement) );
        }
        if ( pole.endI >= (int)(ranges.size())-numReturnsEitherSide )
        {
            minDist = MIN( minDist, pole.range * ((ranges.size()-1-pole.endI)*angleIncrement) );
        }
        
        // Check for clutter near the pole:
        // before
        int startBefore = MAX( 0, pole.startI-numReturnsEitherSide );
        for ( int i=startBefore; i < pole.startI; i++ )
        {
            minDist = MIN( minDist, dist( i, pole.startI, ranges, angleIncrement ) );
        }
        // after
        int endAfter = MIN( (int)(ranges.size())-1, pole.endI+numReturnsEitherSide );
        for ( int i=pole.endI+1; i <= endAfter; i++ )
        {
            minDist = MIN( minDist, dist( i, pole.endI, ranges, angleIncrement ) );
        }

        assert( minDist >= 0 );
        return minDist;
    }
}

//////////////////////////////////////////////////////////////////////

bool
ForegroundExtractor::Config::isSane() const
{
    if ( !scannerConfig.isSane() ||
         minForegroundWidth < 0 ||
         maxForegroundWidth < minForegroundWidth ||
         minForegroundBackgroundSeparation < 0 ||
         minForegroundBackgroundSeparation > clearForegroundBackgroundSeparation )
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::string toString( const ForegroundExtractor::Config &c )
{
    stringstream ss;
    ss << endl;
    ss << "  ScannerConfig: " << toString(c.scannerConfig) << endl
       << "  minForegroundWidth: " << c.minForegroundWidth << endl
       << "  maxForegroundWidth: " << c.maxForegroundWidth << endl
       << "  minForegroundBackgroundSeparation: " << c.minForegroundBackgroundSeparation << endl
       << "  clearForegroundBackgroundSeparation: " << c.clearForegroundBackgroundSeparation;
    return ss.str();
}

//////////////////////////////////////////////////////////////////////

std::vector<AnnotatedPole>
ForegroundExtractor::extractForegroundPoints( const std::vector<float> &ranges, bool debug ) const
{
    possiblePoles_.clear();

    possiblePoles_ = detectPoles( ranges,
                                  config_.scannerConfig,
                                  config_.minForegroundWidth,
                                  config_.maxForegroundWidth,
                                  config_.minForegroundBackgroundSeparation,
                                  debug );

    if ( debug )
    {
        cout<<"TRACE(foregroundextractor.cpp): possible poles:" << endl;
        for ( size_t i=0; i < possiblePoles_.size(); i++ )
            cout << "  " << toString(possiblePoles_[i]) << endl;
    }

    std::vector<AnnotatedPole> annotatedPoles;
    for ( size_t i=0; i < possiblePoles_.size(); i++ )
    {
        double distToNearestClutter = 
            distToClutter( possiblePoles_[i],
                           ranges,
                           config_.scannerConfig.startAngle,
                           config_.scannerConfig.angleIncrement,
                           config_.clearForegroundBackgroundSeparation );

        if ( distToNearestClutter < config_.minForegroundBackgroundSeparation )
            continue;

        annotatedPoles.push_back( AnnotatedPole( possiblePoles_[i], distToNearestClutter ) );
    }
    return annotatedPoles;
}

std::vector<hydrofeatureobs::PointFeatureObs*>
ForegroundFeatureExtractor::extractFeatures( const std::vector<float>         &ranges,
                                             const std::vector<AnnotatedPole> &fgPoints ) const
{
    std::vector<hydrofeatureobs::PointFeatureObs*> features;
    for ( size_t i=0; i < fgPoints.size(); i++ )
    {
        double pTruePos, pFalsePos;
        calcForegroundPointReliability( ranges,
                                        fgPoints[i],
                                        config_,
                                        pTruePositive_,
                                        pFalsePositive_,
                                        pTruePos,
                                        pFalsePos );

        if ( (pTruePos-pFalsePos) < 0.05 ) continue;

        double rangeSd   = rangeSd_ + (fgPoints[i].pole.width)/3.0;
        double bearingSd = bearingSd_ + (fgPoints[i].pole.width/fgPoints[i].pole.range)/3.0;
        features.push_back( new hydrofeatureobs::PointFeatureObs( hydrofeatureobs::FOREGROUNDPOINT,
                                                                  pFalsePos,
                                                                  pTruePos,
                                                                  fgPoints[i].pole.range,
                                                                  fgPoints[i].pole.bearing,
                                                                  rangeSd,
                                                                  bearingSd ) );
    }
    return features;
}

//////////////////////////////////////////////////////////////////////

void
calcForegroundPointReliability( const std::vector<float>          &ranges,
                                const AnnotatedPole               &fgPoint,
                                const ForegroundExtractor::Config &config,
                                double                             basePTruePositive,
                                double                             basePFalsePositive,
                                double                            &pTruePositive,
                                double                            &pFalsePositive )
{
    pTruePositive  = basePTruePositive;
    pFalsePositive = basePFalsePositive;

    // If a pole is close to clutter, don't trust it so much.
    assert ( fgPoint.distToClutter >= config.minForegroundBackgroundSeparation );
    if ( fgPoint.distToClutter < config.clearForegroundBackgroundSeparation )
    {
        double scale = 
            config.clearForegroundBackgroundSeparation -
            config.minForegroundBackgroundSeparation;
        double goodness = 
            (fgPoint.distToClutter - config.minForegroundBackgroundSeparation)/scale;
            
        // cout<<"TRACE(foregroundextractor.cpp): pole: " << toString(fgPoint) << ": goodness: " << goodness << endl;
        // cout<<"TRACE(foregroundextractor.cpp): base: pFalsePos/pTruePos: " << pFalsePos << "/" << pTruePos << endl;

        if ( goodness < 1 )
        {
            const double diff = pTruePositive - pFalsePositive;
            pFalsePositive += (1-goodness) * diff/2.0;
            pTruePositive  -= (1-goodness) * diff/2.0;

            // cout<<"TRACE(foregroundextractor.cpp):  --> pFalsePos/pTruePos: " << pFalsePos << "/" << pTruePos << endl;
        }
    }

    // If a pole consists of just a single point, don't trust it so much.
    if ( fgPoint.pole.startI == fgPoint.pole.endI )
    {
        //const double diff = pTruePositive_ - pFalsePositive_;
        const double diff = pTruePositive - pFalsePositive;
        pFalsePositive += diff/4.0;
        pTruePositive  -= diff/4.0;
    }
        
    // If there are non-returns either side, maybe this is a specular surface on which
    // we've gotten one lucky return.
    if ( (fgPoint.pole.startI > 0 &&
          fgPoint.pole.endI   < (int)(ranges.size())-1) &&
         ( !isValidRange( fgPoint.pole.startI-1, config.scannerConfig ) ||
           !isValidRange( fgPoint.pole.endI+1, config.scannerConfig ) ) )
    {
        //const double diff = pTruePositive_ - pFalsePositive_;
        const double diff = pTruePositive - pFalsePositive;
        pFalsePositive += diff/3.0;
        pTruePositive  -= diff/3.0;            
    }

    // At short ranges, you get fewer false positives
    if ( fgPoint.pole.range < config.reliableScannerRange )
    {
        pFalsePositive = MAX( 0.0, pFalsePositive-0.1 );
    }    
}

//////////////////////////////////////////////////////////////////////

std::string toString( const AnnotatedPole &p )
{
    stringstream ss;
    ss << "{ pole: " << toString(p.pole) << ", distToClutter: " << p.distToClutter << "}";
    return ss.str();
}

}
