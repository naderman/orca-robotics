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

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/systemstatus.h>
#include "componentmonitor.h"

namespace systemstatus
{
    
struct PlatformComponentPair
{
    std::string platformName;
    std::string componentName;
};
    
class MainThread : public orcaice::SubsystemThread
{
public:

    MainThread( const orcaice::Context & context );
    
private:
    
    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    hydroiceutil::JobQueuePtr jobQueue_;
    orcaice::Context context_;
    std::vector<ComponentMonitor> monitors_;    
    orcaifaceimpl::SystemStatusImplPtr systemStatusIface_;
    
    std::vector<PlatformComponentPair> getPlatformComponentPairs();
    void createMonitors();

};

} // namespace

#endif
