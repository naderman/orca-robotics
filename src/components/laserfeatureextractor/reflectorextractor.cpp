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
#include "reflectorextractor.h"
#include <iostream>
#include <assert.h>
#include <cmath>
#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;

namespace laserfeatures {

ReflectorExtractor::ReflectorExtractor( const orca::RangeScanner2dDescription &laserDescr,
                                        const orcaice::Context &context )
{
    hydrolfextract::ReflectorExtractor::Config cfg( getLaserConfig(laserDescr) );

    std::string prefix = context.tag() + ".Config.Reflectors.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.maxDeltaRangeWithinReflector      =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWithinReflector", 0.2 );
    cfg.maxDeltaRangeNearReflector        =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeNearReflector", 0.3 );
    cfg.minReflectorBrightness            =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"MinBrightness", 1);

    cfg.pFalsePositive           =
        orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PFalsePositive", 0.1);
    cfg.pTruePositive            =
        orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PTruePositive", 0.6);

    prefix = context.tag() + ".Config.";
    prop = context.properties();
    cfg.rangeSd   = orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetRangeSd", 0.2);
    cfg.bearingSd = (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetBearingSd", 5.0);    

    reflectorExtractor_.reset( new hydrolfextract::ReflectorExtractor( cfg ) );
}

void 
ReflectorExtractor::addFeatures( const orca::LaserScanner2dDataPtr    &laserData,
                                 orca::PolarFeature2dData &features )
{
    assert( laserData != 0 );

    std::vector<hydrofeatureobs::PointFeatureObs> hydroFeatures =
        reflectorExtractor_->extractFeatures( laserData->ranges, laserData->intensities );

    for ( uint i=0; i < hydroFeatures.size(); i++ )
    {
        features.features.push_back( convert( hydroFeatures[i] ) );
    }
}

}
