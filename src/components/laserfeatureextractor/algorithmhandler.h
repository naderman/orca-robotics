/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
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
