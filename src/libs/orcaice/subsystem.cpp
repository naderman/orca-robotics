/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "subsystem.h"

using namespace orcaice;

Subsystem::Subsystem( gbxutilacfr::Tracer& tracer, gbxutilacfr::Status& status, const std::string& subsysName ) : 
    SubsystemThread( tracer, status, subsysName )
{
}

void 
Subsystem::walk()
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
