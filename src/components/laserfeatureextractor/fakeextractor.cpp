#include "fakeextractor.h"
#include "featureextraction.h"

#include <orcaiceutil/objutils.h>
#include <iostream>

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

int FakeExtractor::computeFeatures( const orca::LaserConfigPtr laserConfigPtr, const orca::LaserDataPtr laserDataPtr, PolarFeature2dDataPtr featureDataPtr)
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
