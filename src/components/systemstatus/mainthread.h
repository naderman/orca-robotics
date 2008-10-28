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
#include <orcaifaceimpl/systemstatusImpl.h>
#include "componentmonitor.h"

namespace systemstatus
{
    
class MainThread : public orcaice::SubsystemThread
{
public:

    MainThread( const orcaice::Context & context );
    
private:
    
    // from SubsystemThread
    virtual void walk();

    hydroiceutil::JobQueuePtr jobQueue_;
    orcaice::Context context_;
    std::vector<ComponentMonitor> monitors_;    
    orcaifaceimpl::SystemStatusImplPtr systemStatusIface_;
    
    // using a map assumes that there's only one status interface per component
    std::map<std::string,std::string> getComponentPlatformPairs();
    void createMonitors();

};

} // namespace

#endif
