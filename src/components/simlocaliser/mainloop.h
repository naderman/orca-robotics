/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_STAGE_LOCALISER_MAIN_LOOP_H
#define ORCA2_STAGE_LOCALISER_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orca/localise2d.h>

#include "driver.h"

namespace simlocaliser
{

// Note: this thing self-destructs when run() returns.
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( const orca::Localise2dConsumerPrx                localise2dConsumer,
              orcaice::Buffer<orca::Localise2dData>     &locBuffer,
              const orcaice::Context & context );
    virtual ~MainLoop();

    virtual void run();

private:

    // IceStorm consumer
    const orca::Localise2dConsumerPrx  localise2dConsumer_;
    
    // outgoing
    orcaice::Buffer<orca::Localise2dData> &locBuffer_;

    // generic interface to the hardware
    Driver* driver_;
    
    orcaice::Context context_;
};

} // namespace

#endif
