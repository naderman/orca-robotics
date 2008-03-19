/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "lineextractor.h"
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "util.h"
#include <iostream>
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>

#define REJECT_GROUND_OBSERVATIONS 1  // 0  1

#define MIN_DELTA_RANGE_FOR_LINE_ENDPOINT 0.2

using namespace std;

namespace laserfeatures {

LineExtractor::LineExtractor( const orca::RangeScanner2dDescription &laserDescr,
                              bool extractLines,
                              bool extractCorners,
                              const orcaice::Context &context )
{
    assert( extractLines || extractCorners );

    hydrolfextract::LineExtractor::Config cfg( getLaserConfig(laserDescr), extractLines, extractCorners );

    std::string prefix = context.tag() + ".Config.Lines.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.clusterMaxRangeDelta = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"ClusterMaxRangeDelta", 0.5 );
    cfg.breakDistThreshold   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"BreakDistThreshold", 0.2 );
    cfg.minPointsInLine      = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinPointsInLine", 6 );
    cfg.minLineLength        = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinLineLength", 1.0 );
    cfg.rejectLikelyGroundObservations = orcaice::getPropertyAsIntWithDefault( prop, prefix+"RejectLikelyGroundObservations", true );
    cfg.linePFalsePositive   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.4 );
    cfg.linePFalsePositivePossibleGround = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositivePossibleGround", 0.55 );
    cfg.linePTruePositive = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.6 );


    cfg.cornerPFalsePositive   = orcaice::getPropertyAsDoubleWithDefault( prop, "Config.Corners.PFalsePositive", 0.2 );
    cfg.cornerPTruePositive = orcaice::getPropertyAsDoubleWithDefault( prop, "Config.Corners.PTruePositive", 0.6 );

    cfg.rangeSd        = orcaice::getPropertyAsDoubleWithDefault( prop, "Config.RangeSd", 0.2 );
    cfg.bearingSd      = (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault( prop, "Config.BearingSd", 5.0 );

    lineExtractor_.reset( new hydrolfextract::LineExtractor( cfg, context.toHydroContext() ) );
}
    
void LineExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                 orca::PolarFeature2dData &features )
{
    std::vector<hydrofeatureobs::FeatureObs*> hydroFeatures =
        lineExtractor_->extractFeatures( laserData->ranges );

    for ( uint i=0; i < hydroFeatures.size(); i++ )
    {
        features.features.push_back( convert( *hydroFeatures[i] ) );
        delete hydroFeatures[i];
    }
}

}
