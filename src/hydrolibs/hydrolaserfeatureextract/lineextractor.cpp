/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "lineextractor.h"
#include <iostream>
#include <gbxutilacfr/mathdefs.h>
#include <sstream>
#include <hydroutil/hydroutil.h>

using namespace std;

namespace hydrolfextract {

////////////////////////////////////////////////////////////////////////////////

bool
LineExtractor::Config::isSane() const
{
    return ( scannerConfig.isSane() &&
             maxDeltaRangeInCluster > 0 &&
             maxDeltaInLine > 0 &&
             minPointsInLine > 1 &&
             minLineLengthForVisibleEndpoints > 0 );
}

AnnotatedLines
LineExtractor::extractLines( const std::vector<float> &ranges ) const
{
    Lines lines = hydrolfextract::extractLines( ranges, 
                                                config_.scannerConfig,
                                                config_.minPointsInLine,
                                                config_.maxDeltaRangeInCluster,
                                                config_.maxDeltaInLine );
    AnnotatedLines annLines = getAnnotatedLines( lines, ranges, config_.minLineLengthForVisibleEndpoints );
    return annLines;
}

std::vector<hydrofeatureobs::PointFeatureObs*> 
CornerFeatureExtractor::extractFeatures( const AnnotatedLines &annLines ) const
{
    std::vector<hydrofeatureobs::PointFeatureObs*> features;

    for ( size_t i=0; i < annLines.numClusters(); i++ )
    {
        const AnnotatedCluster &annCluster = annLines.clusters()[i];

        // Note: start on second line in cluster
        for ( size_t j=1; j < annCluster.numLineSegments(); j++ )
        {
            if ( ( annCluster.startVisibility(j) == VisibleInternalLineJunction ||
                   annCluster.startVisibility(j) == VisibleExternalLineJunction ) )
            {
                if ( annCluster.lastPointInLine(j-1).returnI() == annCluster.firstPointInLine(j).returnI() )
                {
                    // Lines share a common point in the corner
                    int featureType;
                    if ( annCluster.startVisibility(j) == VisibleInternalLineJunction )
                        featureType = hydrofeatureobs::INTERNALCORNER;
                    else
                        featureType = hydrofeatureobs::EXTERNALCORNER;

                    features.push_back( new hydrofeatureobs::PointFeatureObs( featureType,
                                                                              pFalsePositive_,
                                                                              pTruePositive_,
                                                                              annCluster.firstPointInLine(j).range(),
                                                                              annCluster.firstPointInLine(j).bearing(),
                                                                              rangeSd_,
                                                                              bearingSd_ ) );
                }
                else
                {
                    cout<<"TRACE(lineextractor.cpp): Huh? How come there's a corner but they don't share a common point??" << endl;
                    assert( false );
                }
            }
        }
    }

    return features;
}

std::vector<hydrofeatureobs::LineFeatureObs*> 
LineFeatureExtractor::extractFeatures( const AnnotatedLines &annLines ) const
{
    std::vector<hydrofeatureobs::LineFeatureObs*> features;

    for ( size_t i=0; i < annLines.numClusters(); i++ )
    {
        const AnnotatedCluster &annCluster = annLines.clusters()[i];

        for ( size_t j=0; j < annCluster.numLineSegments(); j++ )
        {
            if ( annCluster.lineLength(j) < minLineLength_ )
                continue;

            // Project the end-points onto the line of best fit
            const geom2d::Point start = geom2d::closestPointOnLine( annCluster.lines()[j].bestFitLine(),
                                                                    annCluster.firstPointInLine(j).point() );
            const geom2d::Point end = geom2d::closestPointOnLine( annCluster.lines()[j].bestFitLine(),
                                                                  annCluster.lastPointInLine(j).point() );
            const bool startVisible = isVisible( annCluster.startVisibility(j) );
            const bool endVisible = isVisible( annCluster.endVisibility(j) );

            features.push_back( new hydrofeatureobs::LineFeatureObs( hydrofeatureobs::LINE,
                                                                     pFalsePositive_,
                                                                     pTruePositive_,
                                                                     start.range(), start.bearing(),
                                                                     end.range(), end.bearing(),
                                                                     startVisible,
                                                                     endVisible,
                                                                     rangeSd_+annCluster.lineFitRhoSd(j),
                                                                     bearingSd_+annCluster.lineFitAlphaSd(j) ) );
        }
    }

    return features;
}

}

