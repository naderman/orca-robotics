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

int FakeExtractor::computeFeatures( PolarFeature2dDataPtr features)
{
    // make up some features
//     PolarPoint2d p;
//     p.r = 1.0;
//     p.o = -M_PI;
//     
//     features->features.r=1.0;
    
    return 0;
}
