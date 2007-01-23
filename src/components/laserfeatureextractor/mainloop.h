/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_FEATURE_EXTRACTOR_MAIN_LOOP_H
#define ORCA2_FEATURE_EXTRACTOR_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>

#include <orca/polarfeature2d.h>
#include <orca/laserscanner2d.h>
#include <orcaifaceimpl/polarfeature2dI.h>

namespace laserfeatures
{

class AlgorithmDriver;

class MainLoop : public orcaice::Thread
{
public:

    MainLoop( orcaifaceimpl::PolarFeature2dI &featureInterface,
              const orcaice::Context &context );

//         const orca::PolarFeature2dConsumerPrx &polarFeaturesConsumer,
//         orca::LaserScanner2dPrx laserPrx,
//         orcaice::PtrBuffer<orca::LaserScanner2dDataPtr> &laserDataBuffer,
//         orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> &polarFeaturesDataBuffer,
//         const orcaice::Context & context );

    ~MainLoop();

    virtual void run();

private:

    void connectToLaser();
    void getLaserDescription();
    void initInterface();
    void initDriver();
    
    // This component is 2D-centric: can only handle certain orientations.
    bool sensorOffsetOK( const orca::Frame3d & offset );

    void convertToRobotCS( const orca::PolarFeature2dDataPtr & featuresPtr );

    // generic algorithm
    AlgorithmDriver* driver_;
    
    // Our external interface
    orcaifaceimpl::PolarFeature2dI &featureInterface_;
    
    // Laser proxy
    orca::LaserScanner2dPrx laserPrx_;
    
    // Laser config and geometry
    orca::Frame3d sensorOffset_;    

    // Description of laser details
    orca::RangeScanner2dDescription laserDescr_;

    // Proxy for incoming scans
    orcaice::PtrProxy<orca::LaserScanner2dDataPtr> laserDataProxy_;
    
    orcaice::Context context_;

};

} // namespace

#endif
