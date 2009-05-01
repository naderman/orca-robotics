/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef COMPONENT_MONITOR
#define COMPONENT_MONITOR

#include <hydroiceutil/jobqueue.h>

#include <orca/systemstatus.h>
#include "statusconsumerImpl.h"

using namespace std;

namespace systemstatus {
    
//
// Monitors individual component's status interface
// Wraps up the StatusConsumer interface
//
// Author: Tobias Kaupp
//
class ComponentMonitor
{
    public:
        ComponentMonitor( const orca::FQComponentName& compName,
                          hydroiceutil::JobQueuePtr  jobQueue,
                          const orcaice::Context    &context );
        
        // Get the current component status
        orca::EstimatedComponentStatus getComponentStatus();
        
    private:
        StatusConsumerImplPtr statusConsumer_;

        orca::FQComponentName compName_;
        hydroiceutil::JobQueuePtr jobQueue_;
        orcaice::Context context_;
};

}

#endif

