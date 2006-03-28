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

#ifndef ORCA2_FEATURE_EXTRACTOR_ALGORITHM_HANDLER_H
#define ORCA2_FEATURE_EXTRACTOR_ALGORITHM_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <orca/polarfeature2d.h>
#include <orca/laser.h>

namespace laserfeatures
{

class AlgorithmDriver;

class AlgorithmHandler : public orcaice::Thread
{
public:

    AlgorithmHandler( const orca::PolarFeature2dConsumerPrx &polarFeaturesConsumer,
                    orca::LaserPrx laserPrx,
                    orcaice::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer,
                    orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> &polarFeaturesDataBuffer,
                    const orcaice::Context & context );
    ~AlgorithmHandler();

    virtual void run();

private:
    
    // generic algorithm
    AlgorithmDriver* driver_;
    
    // IceStorm consumer
    const orca::PolarFeature2dConsumerPrx &polarFeaturesConsumer_; 
    
    // Laser proxy
    orca::LaserPrx laserPrx_;
    
    // Laser config and geometry
    orca::RangeScannerConfigPtr laserConfigPtr_;
    orca::RangeScannerGeometryPtr laserGeometryPtr_;
    
    // buffers
    orcaice::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer_;
    orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> &polarFeaturesDataBuffer_;
    
    void convertToRobotCS( const orca::PolarFeature2dDataPtr & featuresPtr );

    // This component is 2D-centric: can only handle certain orientations.
    // Throws std::strings on bad offset.
    void checkLaserGeometry( const orca::RangeScannerGeometryPtr geom );

    orcaice::Context context_;

    void init();
};

} // namespace

#endif
