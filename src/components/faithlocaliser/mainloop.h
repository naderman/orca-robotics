/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_FAITH_LOCALISER_MAIN_LOOP_H
#define ORCA2_FAITH_LOCALISER_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/buffer.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/localise2diface.h>

namespace faithlocaliser
{

// Note: this thing self-destructs when run() returns.
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( const orcaice::Context &context );
    ~MainLoop();

    virtual void run();

private:
    
    void initNetwork();
    
    orca::Odometry2dPrx odometryPrx_;
    orcaice::Buffer<orca::Odometry2dData> odometryPipe_;
    
    orcaifaceimpl::Localise2dIfacePtr localiseInterface_;

    orcaice::Context context_;

    double stdDevPosition_;
    double stdDevHeading_;
};

} // namespace

#endif
