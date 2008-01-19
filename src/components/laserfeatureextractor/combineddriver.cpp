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
#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>

#include "reflectorextractor.h"
#include "foregroundextractor.h"
#include "doorextractor.h"
#include "lineextractor.h"

#include "combineddriver.h"

using namespace std;
using namespace orcaice;

namespace laserfeatures {

CombinedDriver::CombinedDriver( const orcaice::Context & context, double maxRange, int numberOfSamples )
{
    std::string prefix = context.tag() + ".Config.";
    Ice::PropertiesPtr prop = context.properties();

    bool extractReflectors                 =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractReflectors", 1);
    bool extractForegroundPoints           =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractForegroundPoints", 0);
    bool extractLines                      =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractLines", 0);
    bool extractCorners                    =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractCorners", 0);
    bool extractDoors                      =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractDoors", 0);
    
    if ( extractReflectors )
        extractors_.push_back( new ReflectorExtractor(context,maxRange,numberOfSamples) );
    if ( extractForegroundPoints )
        extractors_.push_back( new ForegroundExtractor(context,maxRange) );
    if ( extractDoors )
        extractors_.push_back( new DoorExtractor(context) );
    if ( extractCorners || extractLines ) 
        extractors_.push_back( new LineExtractor(context,maxRange,extractLines,extractCorners) );

    if ( extractors_.size() == 0 )
    {
        cout << "ERROR(combineddriver.cpp): No feature extractors defined in .cfg file!" << endl;
        exit(1);
    }
}

CombinedDriver::~CombinedDriver()
{
    for ( unsigned int i=0; i < extractors_.size(); i++ )
    {
        delete extractors_[i];
    }
}

int
CombinedDriver::computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                 orca::PolarFeature2dData          &featureData )
{
    featureData.features.clear();

    for ( unsigned int i=0; i < extractors_.size(); i++ )
    {
        extractors_[i]->addFeatures( laserDataPtr, featureData );
    }
  
    for ( unsigned int i=0; i < featureData.features.size(); i++ )
    {
        assert ( featureData.features[i]->pTruePositive > featureData.features[i]->pFalsePositive );
    }

    return 0;
}

}
