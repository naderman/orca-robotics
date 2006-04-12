#include "fakemaploader.h"
#include <iostream>

using namespace orca;
using namespace std;

namespace featuremaploader {

void fakeLoadMap( orca::FeatureMap2dDataPtr theMap )
{
    theMap->features.clear();

    CartesianPointFeature2dPtr f1 = new CartesianPointFeature2d;
    f1->type = orca::feature::LASERREFLECTOR;
    f1->p.x  = 0.0;
    f1->p.y  = 0.0;
    theMap->features.push_back( f1 );

    CartesianPointFeature2dPtr f2 = new CartesianPointFeature2d;
    f2->type = orca::feature::LASERREFLECTOR;
    f2->p.x  = 20.0;
    f2->p.y  = 20.0;
    theMap->features.push_back( f2 );
}

}
