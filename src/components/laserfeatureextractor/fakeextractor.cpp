/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "fakeextractor.h"

#include <orcaice/objutils.h>
#include <iostream>
#include <math.h>

// For definitions of feature types:
#include <orca/featuremap2d.h>


using namespace orca;
using namespace std;
using namespace orcaice;

FakeExtractor::FakeExtractor()
{
}

FakeExtractor::~FakeExtractor()
{
}

int FakeExtractor:: initialize( ConfigParameters *configParameters )
{
    return 0;
}

int FakeExtractor::computeFeatures( const orca::RangeScannerConfigPtr laserConfigPtr,
                                    const orca::LaserDataPtr laserDataPtr,
                                    PolarFeature2dDataPtr featureDataPtr)
{
    // make up some features
    cout << "INFO(fakeextractor.cpp): Laser configuration received: " << laserConfigPtr << endl;
    cout << "INFO(fakeextractor.cpp): Laser scan size received: " << laserDataPtr->ranges.size() << endl << endl;

    cout<<"TRACE(fakeextractor.cpp): initially, numFeatures = " << featureDataPtr->features.size() << endl;
    
    double range = 1.0;
    double bearing = -M_PI;

    cout<<"TRACE(fakeextractor.cpp): resizing." << endl;
    
    featureDataPtr->features.resize(10);

    cout<<"TRACE(fakeextractor.cpp): done resize" << endl;

    for (uint i=0; i<10; i++)
    {
        featureDataPtr->features[i] = new SinglePolarFeature2d;
        featureDataPtr->features[i]->type = orca::feature::FOREGROUNDPOINT;
        featureDataPtr->features[i]->p.r  = range;
        featureDataPtr->features[i]->p.o  = bearing;
        range = range + 2.0;
        bearing = bearing + M_PI/5.0;
    }
    
    return 0;
}
