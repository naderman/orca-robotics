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
#include <cmath>
#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>

#include "fakedriver.h"

using namespace std;
using namespace orcaice;
using namespace laserfeatures;

FakeDriver::FakeDriver()
{
}

int FakeDriver::computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                       orca::PolarFeature2dData    &featureData )
{
    // make up some features
    cout << "INFO(fakeextractor.cpp): Laser scan size received: " << laserDataPtr->ranges.size() << endl << endl;

    cout<<"TRACE(fakeextractor.cpp): initially, numFeatures = " << featureData.features.size() << endl;
    
    double range = 1.0;
    double bearing = -M_PI;

    for (unsigned int i=0; i<10; i++)
    {
        orca::PointPolarFeature2dPtr f = new orca::PointPolarFeature2d;
        f->type = orca::feature::FOREGROUNDPOINT;
        f->p.r  = range;
        f->p.o  = bearing;
        featureData.features.push_back( f );
        range = range + 2.0;
        bearing = bearing + M_PI/5.0;
    }
    
    return 0;
}
