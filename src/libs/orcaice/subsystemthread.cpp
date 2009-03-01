/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "subsystemthread.h"

using namespace orcaice;

SubsystemThread::SubsystemThread( gbxutilacfr::Tracer& tracer, gbxutilacfr::Status& status, const std::string& subsysName ) : 
    SubstatusThread( tracer, status, subsysName )
{
}

void 
SubsystemThread::walk()
{
    // init: Idle --> Initialising
    subStatus().initialising();
    initialise();
    
    if ( !isStopping() )
    {
        // finished: Initialising --> Working
        subStatus().working();
        work();

        // finished: Working --> Finalising
        subStatus().finalising();
        finalise();
    }
    else 
    {
        // finished: Initialising --> Finalising
        subStatus().finalising();
        finalise();
    }

    // finished: Finalising --> Shutdown
    subStatus().status().setSubsystemStatus( subStatus().name(), gbxutilacfr::SubsystemShutdown, gbxutilacfr::SubsystemOk );
}
