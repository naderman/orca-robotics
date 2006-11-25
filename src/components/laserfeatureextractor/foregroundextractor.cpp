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

#include <iostream>
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>
#include <orcaobj/mathdefs.h>
#include <orcaice/orcaice.h>
#include "foregroundextractor.h"
#include "polefinder.h"

using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.3;
    const double P_TRUE_POSITIVE  = 0.5;
}

ForegroundExtractor::ForegroundExtractor( orcaice::Context context, double laserMaxRange )
    : laserMaxRange_( laserMaxRange )
{
    std::string prefix = context.tag() + ".Config.FGPoints.";
    Ice::PropertiesPtr prop = context.properties();

    minForegroundWidth_                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundWidth", 0.1);
    maxForegroundWidth_                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxForegroundWidth", 0.5);
    minForegroundBackgroundSeparation_ =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundBackgroundSeparation", 0.5);
}

void ForegroundExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData, 
                                       orca::PolarFeature2dDataPtr &features )
{
    assert( laserMaxRange_ > 0.0 );
    
    std::vector<orca_polefinder::positionRB> poles;

    double startAngleFromDodge = DEG2RAD( 2.0 );

    int numPoles = orca_polefinder::detect_poles(
            laserData,
            laserMaxRange_,
            minForegroundWidth_,
            maxForegroundWidth_,
            minForegroundBackgroundSeparation_,
            startAngleFromDodge,
            poles );

    for ( int i=0; i < numPoles; i++ )
    {
        orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
        pp->type = orca::feature::FOREGROUNDPOINT;
        pp->p.r  = poles[i].range;
        pp->p.o  = poles[i].bearing;
        pp->pFalsePositive = P_FALSE_POSITIVE;
        pp->pTruePositive = P_TRUE_POSITIVE;
        features->features.push_back(pp);
    }

}

}
