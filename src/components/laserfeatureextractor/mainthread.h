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

#ifndef ORCA_FEATURE_EXTRACTOR_MAIN_LOOP_H
#define ORCA_FEATURE_EXTRACTOR_MAIN_LOOP_H

#include <hydroiceutil/subsystemthread.h>
#include <orcaice/context.h>

#include <orca/laserscanner2d.h>
#include <orcaifaceimpl/polarfeature2dImpl.h>
#include <orcaifaceimpl/storingconsumers.h>
#include "driver.h"

namespace laserfeatures
{

class MainThread : public hydroiceutil::SubsystemThread
{
public:

    MainThread( const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:

    void connectToLaser();
    void getLaserDescription();
    void initDriver();
    void initNetworkInterface();
    
    // This component is 2D-centric: can only handle certain orientations.
    bool sensorOffsetOK( const orca::Frame3d & offset );

    void convertToRobotCS( const orca::PolarFeature2dData &featuresPtr );

    // generic algorithm
    std::auto_ptr<Driver> driver_;
    
    // Our external interface
    orcaifaceimpl::PolarFeature2dImplPtr featureInterface_;
    
    // Laser proxy
    orca::LaserScanner2dPrx laserPrx_;
    
    // Laser config and geometry
    orca::Frame3d sensorOffset_;    

    // Description of laser details
    orca::RangeScanner2dDescription laserDescr_;

    // If the driver's fake, don't need to make external conenctions.
    bool fakeDriver_;

    // Consumers
    orcaifaceimpl::StoringRangeScanner2dConsumerImplPtr laserConsumer_;

    orcaice::Context context_;
};

} // namespace

#endif
