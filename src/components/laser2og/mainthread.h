/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_LASER2OG_HANDLER_H_
#define _ORCA2_LASER2OG_HANDLER_H_

#include <hydroutil/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <orca/rangescanner2d.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>

#include "laser2og.h"

namespace laser2og
{

class MainThread : public hydroutil::Thread
{
public:
    
    MainThread( const orcaice::Context &context );
    ~MainThread();

    virtual void run();
    
private:

    void init();
    
    orcaice::Context context_;
    
    orcaice::PtrBuffer<orca::RangeScanner2dDataPtr> rangeScannerDataBuffer_;
    // for our requests for current location
    orca::Localise2dPrx localise2dPrx_;
    // for getting config data
    orca::RangeScanner2dPrx rangeScannerPrx_;
    // for setting ogfusion data
    orca::OgFusionPrx ogFusionPrx_;
    // for receiving the data
    orca::RangeScanner2dConsumerPrx rangeScannerConsumerPrx_;
    Laser2Og *laser2Og_;

};

} //namespace

#endif

