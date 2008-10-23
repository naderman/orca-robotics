/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef COMPONENT_MONITOR
#define COMPONENT_MONITOR

#include <iostream>
#include <hydroiceutil/jobqueue.h>

#include "statusconsumerImpl.h"

using namespace std;

namespace statusmon {
    
//
// Monitors individual component's status interface
// Wraps up the StatusConsumer interface
//
// Author: Tobias Kaupp
//
class ComponentMonitor
{
    public:
        ComponentMonitor( hydroiceutil::JobQueuePtr  jobQueue,
                          const std::string         &platform,
                          const std::string         &component,
                          const orcaice::Context    &context );
        ~ComponentMonitor() {};
        
        StatusDetails getStatus();
        
    private:
        orcaice::Context context_;
        StatusConsumerImpl::Config config_;
        StatusConsumerImplPtr statusConsumer_;
        hydroiceutil::JobPtr job_;  
};


}

#endif

