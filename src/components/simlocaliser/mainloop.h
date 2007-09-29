/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_STAGE_LOCALISER_MAIN_LOOP_H
#define ORCA2_STAGE_LOCALISER_MAIN_LOOP_H

#include <orcaiceutil/thread.h>
#include <orcaice/context.h>

namespace simlocaliser
{

class Driver;

class MainLoop : public orcaiceutil::Thread
{

public:
    MainLoop( const orcaice::Context& context );
    virtual ~MainLoop();

    // from orcaiceutil::Thread
    virtual void run();

private:
    // generic interface to the hardware
    Driver* driver_;
    
    orcaice::Context context_;
};

} // namespace

#endif
