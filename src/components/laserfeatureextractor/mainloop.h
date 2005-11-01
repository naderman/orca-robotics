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
 *  License along with this library; if not, write to the Freorca::LaserConfigPtre Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_FEATUREEXTRACTOR_MAINLOOP_H
#define ORCA2_FEATUREEXTRACTOR_MAINLOOP_H

#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaiceutil/thread.h>
#include <orcaiceutil/ptrbuffer.h>
#include <orca/polarfeature2d.h>
#include <orca/laser.h>

#include "featureextractorbase.h"

#include <string>

class MainLoop : public orcaiceutil::Thread
{
public:

    MainLoop(   FeatureExtractorBase *algorithm,
                const orca::PolarFeature2dConsumerPrx &polarFeaturesConsumer,
                orca::LaserPrx laserPrx,
                orcaiceutil::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer, 
                orcaiceutil::PtrBuffer<orca::PolarFeature2dDataPtr> &polarFeaturesDataBuffer,
                Ice::PropertiesPtr *prop,
                std::string prefix );
    ~MainLoop();

    virtual void run();

private:
    
    // generic algorithm
    FeatureExtractorBase *algorithm_;
    
    // IceStorm consumer
    const orca::PolarFeature2dConsumerPrx &polarFeaturesConsumer_; 
    
    // Laser proxy
    orca::LaserPrx laserPrx_;
    
    // Laser config and geometry
    orca::LaserConfigPtr laserConfigPtr_;
    orca::LaserGeometryPtr laserGeometryPtr_;
    
    // buffers
    orcaiceutil::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer_;
    orcaiceutil::PtrBuffer<orca::PolarFeature2dDataPtr> &polarFeaturesDataBuffer_;
    
    // access to configuration parameters
    Ice::PropertiesPtr *prop_;
    
    std::string prefix_;
    
    void convertToRobotCS(orca::PolarFeature2dDataPtr featuresPtr);
    
};

#endif
