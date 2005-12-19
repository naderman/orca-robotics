/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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
#include "featureextraction.h"

#include <orcaiceutil/objutils.h>
#include <iostream>
#include <math.h>

using namespace orca;
using namespace std;
using namespace orcaiceutil;

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

int FakeExtractor::computeFeatures( const orca::RangeScannerConfigPtr laserConfigPtr, const orca::LaserDataPtr laserDataPtr, PolarFeature2dDataPtr featureDataPtr)
{
    // make up some features
    cout << "INFO(fakeextractor.cpp): Laser configuration received: " << laserConfigPtr << endl;
    cout << "INFO(fakeextractor.cpp): Laser scan size received: " << laserDataPtr->ranges.size() << endl << endl;
    
    double range = 1.0;
    double bearing = -M_PI;
    
    featureDataPtr->features.resize(10);
    featureDataPtr->featureTypes.resize(10);
    
    for (uint i=0; i<10; i++)
    {
        featureDataPtr->features[i].r = range;
        featureDataPtr->features[i].o = bearing;
        featureDataPtr->featureTypes[i] = slamfeatures::FOREGROUND_POINT;
        range = range + 2.0;
        bearing = bearing + M_PI/5.0;
    }   
    
    return 0;
}
