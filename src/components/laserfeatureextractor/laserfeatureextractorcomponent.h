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

#ifndef ORCA2_LASER_FEATURE_EXTRACTOR_COMPONENT_H
#define ORCA2_LASER_FEATURE_EXTRACTOR_COMPONENT_H


#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

// interface definitions
#include <orca/laser.h>
#include <orca/polarfeature2d.h>

// interface implementations
#include "laserconsumer_i.h"
#include "polarfeature2d_i.h"

class MainLoop;
class FeatureExtractorBase;

class LaserFeatureExtractorComponent : public orcaice::Component
{
public:

    LaserFeatureExtractorComponent();
    virtual ~LaserFeatureExtractorComponent();

    // component interface
    virtual void start();
    virtual void stop();

private:

    orca::LaserPrx laserPrx_;
    orca::RangeScannerConsumerPrx laserCallbackPrx_;
    
    // External interface: polar features
    Ice::ObjectPtr polarFeature_;
    orca::PolarFeature2dConsumerPrx polarFeaturePublisher_;
    
    orcaice::PtrBuffer<orca::LaserDataPtr> laserDataBuffer_;
    orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> polarFeaturesDataBuffer_;

    MainLoop* mainLoop_;
    FeatureExtractorBase *algorithm_;
    
    Ice::PropertiesPtr prop_;
    
};

#endif
