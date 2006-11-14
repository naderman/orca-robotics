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

#ifndef ORCA2_FEATURE_EXTRACTOR_ALGORITHM_HANDLER_H
#define ORCA2_FEATURE_EXTRACTOR_ALGORITHM_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <orca/polarfeature2d.h>
#include <orca/laserscanner2d.h>

namespace laserfeatures
{

class AlgorithmDriver;

class AlgorithmHandler : public orcaice::Thread
{
public:

    AlgorithmHandler( const orca::PolarFeature2dConsumerPrx &polarFeaturesConsumer,
                    orca::LaserScanner2dPrx laserPrx,
                    orcaice::PtrBuffer<orca::LaserScanner2dDataPtr> &laserDataBuffer,
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
    orca::LaserScanner2dPrx laserPrx_;
    
    // Laser config and geometry
    orca::Frame3d sensorOffset_;    

    // This component is 2D-centric: can only handle certain orientations.
    // Throws std::strings on bad offset.
    void checkSensorOffset( const orca::Frame3d & offset );

    // buffers
    orcaice::PtrBuffer<orca::LaserScanner2dDataPtr> &laserDataBuffer_;
    orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> &polarFeaturesDataBuffer_;
    
    void convertToRobotCS( const orca::PolarFeature2dDataPtr & featuresPtr );

    orcaice::Context context_;

    void init();
};

} // namespace

#endif
