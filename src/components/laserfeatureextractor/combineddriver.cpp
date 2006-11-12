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
#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>

#include "reflectorextractor.h"
#include "foregroundextractor.h"
#include "doorextractor.h"
#include "cornerextractor.h"

#include "combineddriver.h"

using namespace std;
using namespace orcaice;

namespace laserfeatures {

CombinedDriver::CombinedDriver( const Config &config )
    : reflectorExtractor_( NULL ),
      foregroundExtractor_( NULL ),
      cornerExtractor_( NULL ),
      doorExtractor_( NULL ),
      config_(config)
{
    
    if ( config_.extractReflectors )
    {
        // cout<<"TRACE(combineddriver.cpp): Extracting reflectors: " << config_.extractReflectors<< endl;
        reflectorExtractor_ = new ReflectorExtractor( config.maxDeltaRangeNearReflector,
                config.maxDeltaRangeWithinReflector,
                config.minReflectorBrightness );
    }
    
    if ( config_.extractForegroundPoints ) 
    {
        // cout<<"TRACE(combineddriver.cpp): Extracting fg points: " << config_.extractForegroundPoints<< endl;
        foregroundExtractor_ = new ForegroundExtractor( config.minForegroundWidth,
                config.maxForegroundWidth,
                config.minForegroundBackgroundSeparation);
    }
    
    if ( config_.extractDoors ) 
    {
        cout<<"TRACE(combineddriver.cpp): Extracting fg points: " << config_.extractForegroundPoints<< endl;
        doorExtractor_ = new DoorExtractor();
    }
    
    if ( config_.extractCorners ) 
    {
        cout<<"TRACE(combineddriver.cpp): Extracting fg points: " << config_.extractForegroundPoints<< endl;
        cornerExtractor_ = new CornerExtractor();
    }
}

CombinedDriver::~CombinedDriver()
{
}

void CombinedDriver::setMaxRange( float maxRange )
{ 
    if (reflectorExtractor_!=NULL) {
        reflectorExtractor_->setMaxRange( maxRange );
    }
    if (foregroundExtractor_!=NULL) {
        foregroundExtractor_->setMaxRange( maxRange );
    }
    if (cornerExtractor_!=NULL) {
        cornerExtractor_->setMaxRange( maxRange );
    }
}

int
CombinedDriver::computeFeatures( const orca::RangeScanner2dConfigPtr &laserConfigPtr,
                                 const orca::LaserScanner2dDataPtr          &laserDataPtr,
                                 orca::PolarFeature2dDataPtr       &featureDataPtr )
{
    featureDataPtr->features.clear();
    
    if ( reflectorExtractor_ != NULL )
    {
        // cout<<"TRACE(combineddriver.cpp): Extracting reflectors: " << config_.extractReflectors<< endl;
        reflectorExtractor_->addFeatures( laserDataPtr, featureDataPtr );
    }
    
    if ( foregroundExtractor_ != NULL ) 
    {
        // cout<<"TRACE(combineddriver.cpp): Extracting fg points: " << config_.extractForegroundPoints<< endl;
        foregroundExtractor_->addFeatures( laserDataPtr, laserConfigPtr, featureDataPtr ) ;
    }
    
    if ( doorExtractor_ != NULL ) 
    {
        // cout<<"TRACE(combineddriver.cpp): Extracting doors: " << config_.extractDoors << endl;
        doorExtractor_->addFeatures(laserDataPtr, featureDataPtr);
    }
    
    if ( cornerExtractor_ != NULL ) 
    {
        // cout<<"TRACE(combineddriver.cpp): Extracting corners: " << config_.extractCorners << endl;
        cornerExtractor_->addFeatures(laserDataPtr, featureDataPtr);
    }
  
    return 0;
}

}
