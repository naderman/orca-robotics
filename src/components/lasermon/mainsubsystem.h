/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_SUBSYSTEM_H
#define MAIN_SUBSYSTEM_H

#include <orcaice/subsystem.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/printingconsumers.h>

namespace lasermon
{

class MainSubsystem : public orcaice::Subsystem
{
public:
    MainSubsystem( const orcaice::Context &context );

private:
    // from Subsystem
    virtual void initialise();
//     virtual void work();
//     virtual void finalise();

    orcaifaceimpl::PrintingRangeScanner2dConsumerImplPtr consumer_;

    orcaice::Context context_;
};

} // namespace

#endif
