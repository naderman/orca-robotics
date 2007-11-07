/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_FEATURE_EXTRACTOR_MAIN_LOOP_H
#define ORCA2_FEATURE_EXTRACTOR_MAIN_LOOP_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>

#include <orca/polarfeature2d.h>
#include <orca/laserscanner2d.h>
#include <orcaifaceimpl/polarfeature2dImpl.h>
#include <orcaifaceimpl/proxiedconsumers.h>

namespace laserfeatures
{

class AlgorithmDriver;

class MainLoop : public hydroutil::Thread
{
public:

    MainLoop( orcaifaceimpl::PolarFeature2dImpl &featureInterface,
              const orcaice::Context &context );

    ~MainLoop();

    virtual void run();

private:

    void connectToLaser();
    void getLaserDescription();
    void initInterface();
    void initDriver();
    
    // This component is 2D-centric: can only handle certain orientations.
    bool sensorOffsetOK( const orca::Frame3d & offset );

    void convertToRobotCS( const orca::PolarFeature2dData &featuresPtr );

    // generic algorithm
    AlgorithmDriver* driver_;
    
    // Our external interface
    orcaifaceimpl::PolarFeature2dImpl &featureInterface_;
    
    // Laser proxy
    orca::LaserScanner2dPrx laserPrx_;
    
    // Laser config and geometry
    orca::Frame3d sensorOffset_;    

    // Description of laser details
    orca::RangeScanner2dDescription laserDescr_;

    // If the driver's fake, don't need to make external conenctions.
    bool fakeDriver_;

    // Consumers
    orcaifaceimpl::ProxiedRangeScanner2dConsumerImplPtr laserConsumer_;

    orcaice::Context context_;

};

} // namespace

#endif
