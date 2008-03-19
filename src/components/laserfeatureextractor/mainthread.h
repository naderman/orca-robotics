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

#include <gbxsickacfr/gbxiceutilacfr/subsystemthread.h>
#include <orcaice/context.h>

#include <orca/laserscanner2d.h>
#include <orcaifaceimpl/polarfeature2dImpl.h>
#include <orcaifaceimpl/storingconsumers.h>
#include "driver.h"

namespace laserfeatures
{

class MainThread : public gbxsickacfr::gbxiceutilacfr::SubsystemThread
{
public:

    MainThread( const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:

    void connectToLaser();
    void getLaserDescription();
    void initDriver();
    void initPolarFeatureInterface();

    // generic algorithm
    std::auto_ptr<Driver> driver_;
    
    // Our external interface
    orcaifaceimpl::PolarFeature2dImplPtr featureInterface_;
    
    // Description of laser details
    orca::RangeScanner2dDescription laserDescr_;

    // Consumers
    orcaifaceimpl::StoringRangeScanner2dConsumerImplPtr laserConsumer_;

    orcaice::Context context_;
};

} // namespace

#endif
