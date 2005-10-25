#include "fakeextractor.h"

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

int FakeExtractor::computeFeatures( PolarFeature2dDataPtr featuresPtr)
{
    // make up some features
    
    double range = 1.0;
    double bearing = -M_PI;
    
    featuresPtr->features.resize(10);
    
    for (uint i=0; i<10; i++)
    {
        featuresPtr->features[i].r = range;
        featuresPtr->features[i].o = bearing;
        range = range + 2.0;
        bearing = bearing + M_PI/5.0;
    }   
    
    return 0;
}
