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

#include <iostream>
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
        ComponentMonitor( hydroiceutil::JobQueuePtr  jobQueue,
                          const std::string         &platformName,
                          const std::string         &componentName,
                          const orcaice::Context    &context );
        ~ComponentMonitor();
        
        // Get the current component status
        void getComponentStatus( orca::ObservedComponentStatus &obsCompStat );
        
    private:
        orca::ObservedComponentState currentState_;
        hydroiceutil::JobQueuePtr jobQueue_;
        std::string platformName_;
        std::string componentName_;
        orcaice::Context context_;
        StatusConsumerImplPtr statusConsumer_;
        
        // set the component state based on what we observe locally
        void setObservedState( orca::ObservedComponentStatus &obsCompStat );
        
        // set the component state based on information reported by the Status interface
        void setReportedState( const StatusDetails           &statDetails,
                               orca::ObservedComponentStatus &obsCompStat );
        
        void addSubscribeJob();
        
        bool isHomeInterfaceReachable();
        
};


}

#endif

