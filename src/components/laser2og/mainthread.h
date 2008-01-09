/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <hydroiceutil/subsystemthread.h>
#include <orcaice/context.h>

#include <orca/rangescanner2d.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>
#include <orcaifaceimpl/bufferedconsumers.h>
#include "laser2og.h"

namespace laser2og
{

class MainThread : public hydroiceutil::SubsystemThread
{
public:
    
    MainThread( const orcaice::Context &context );

    virtual void walk();
    
private:

    void init();
    
    // for our requests for current location
    orca::Localise2dPrx localise2dPrx_;
    // for getting config data
    orca::RangeScanner2dPrx rangeScannerPrx_;
    // for setting ogfusion data
    orca::OgFusionPrx ogFusionPrx_;
    // for receiving the data
    orcaifaceimpl::BufferedRangeScanner2dConsumerImplPtr consumer_;

    std::auto_ptr<Laser2Og> laser2Og_;

    orcaice::Context context_;
};

} //namespace

#endif

