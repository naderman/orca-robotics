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

#include <iostream>
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "foregroundextractor.h"
#include "util.h"

using namespace std;

namespace laserfeatures {

ForegroundExtractor::ForegroundExtractor( const orca::RangeScanner2dDescription &laserDescr,
                                          const orcaice::Context                &context )
{
    hydrolfextract::ForegroundExtractor::Config cfg( getLaserConfig(laserDescr) );

    std::string prefix = context.tag() + ".Config.FGPoints.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.minForegroundWidth                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundWidth", 0.1);
    cfg.maxForegroundWidth                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxForegroundWidth", 0.5);
    cfg.minForegroundBackgroundSeparation =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundBackgroundSeparation", 0.5);

    cfg.pFalsePositive =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.3);
    cfg.pTruePositive =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.5);

    prefix = context.tag() + ".Config.";
    prop = context.properties();
    cfg.rangeSd   = orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetRangeSd", 0.2);
    cfg.bearingSd = (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetBearingSd", 5.0);

    foregroundExtractor_.reset( new hydrolfextract::ForegroundExtractor( cfg ) );
}

void ForegroundExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData, 
                                       orca::PolarFeature2dData &features )
{
    std::vector<hydrofeatureobs::PointFeatureObs> hydroFeatures =
        foregroundExtractor_->extractFeatures( laserData->ranges );

    for ( uint i=0; i < hydroFeatures.size(); i++ )
    {
        features.features.push_back( convert( hydroFeatures[i] ) );
    }
}

}
