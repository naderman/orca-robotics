/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

#include <orca/ogfusion.h>
#include <orca/localise2d.h>
#include <orcaifaceimpl/rangescanner2d.h>
#include "laser2og.h"

namespace laser2og
{

class MainThread : public orcaice::SubsystemThread
{
public:
    
    MainThread( const orcaice::Context &context );
    
private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();
    
    // for our requests for current location
    orca::Localise2dPrx localise2dPrx_;
    // for getting config data
    orca::RangeScanner2dPrx rangeScannerPrx_;
    // for setting ogfusion data
    orca::OgFusionPrx ogFusionPrx_;
    // for receiving the data
    orcaifaceimpl::BufferedRangeScanner2dConsumerImplPtr rangeScannerConsumer_;

    std::auto_ptr<Laser2Og> laser2Og_;

    orcaice::Context context_;
};

} //namespace

#endif

