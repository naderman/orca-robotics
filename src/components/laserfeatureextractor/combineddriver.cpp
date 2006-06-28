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
CombinedDriver::computeFeatures( const orca::RangeScannerConfigPtr &laserConfigPtr,
                                 const orca::LaserDataPtr          &laserDataPtr,
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
