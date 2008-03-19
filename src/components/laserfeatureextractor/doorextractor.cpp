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

#include "doorextractor.h"
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <iostream>
#include <cmath>
#include "util.h"
#include <orca/featuremap2d.h>

using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.3;
    const double P_TRUE_POSITIVE  = 0.6;
}

DoorExtractor::DoorExtractor( const orca::RangeScanner2dDescription &laserDescr,
                              const orcaice::Context &context )
{
    hydrolfextract::DoorExtractor::Config cfg( getLaserConfig( laserDescr ) );
    cout<<"TRACE(doorextractor.cpp): TODO: CONFIG" << endl;

    std::string prefix = context.tag() + ".Config.Doors.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.minDoorWidth = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinDoorWidth", 0.9 );
    cfg.maxDoorWidth = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDoorWidth", 1.2 );
    
    cfg.minDeltaRangeThruDoor = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinDeltaRangeThruDoor", 2.0 );

    cfg.rangeSd   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"RangeSd", 0.3 );
    cfg.bearingSd = M_PI/180.0*orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"BearingSd", 5.0 );

    cfg.minReturnsThruDoor = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinReturnsThruDoor", 3 );
    cfg.minReturnsBesideDoor = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinReturnsBesideDoor", 2 );

    cfg.maxDeltaRangeWallBesideDoor = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWallBesideDoor", 0.3 );

    doorExtractor_.reset( new hydrolfextract::DoorExtractor( cfg, context.toHydroContext() ) );
}


void DoorExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                 orca::PolarFeature2dData &features )
{
    std::vector<hydrofeatureobs::PointFeatureObs> hydroFeatures =
        doorExtractor_->extractFeatures( laserData->ranges );

    for ( uint i=0; i < hydroFeatures.size(); i++ )
    {
        features.features.push_back( convert( hydroFeatures[i] ) );
    }
}

}
