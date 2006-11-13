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
#include "foregroundextractor.h"

#include <iostream>
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>
#include <orcaobj/mathdefs.h>

#include "polefinder.h"

using namespace std;

namespace laserfeatures {
    
void ForegroundExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData, 
                                       const orca::RangeScanner2dConfigPtr &laserConfig,
                                       orca::PolarFeature2dDataPtr &features )
{
    assert( laserMaxRange_ > 0.0 );
    
    std::vector<orca_polefinder::positionRB> poles;

    double startAngleFromDodge = DEG2RAD( 2.0 );

    int numPoles = orca_polefinder::detect_poles( laserConfig,
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
        features->features.push_back(pp);
    }

}

}
