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

// #include <iostream>

// using namespace std;
using namespace orcaice;

Subsystem::Subsystem( const orcaice::Context &context, const std::string& subsysName ) : 
    SubsystemThread( context.tracer(), context.status(), subsysName ),
    context_(context)
{
}

void 
Subsystem::initialise() 
{
}

void 
Subsystem::work() 
{
}

void 
Subsystem::finalise() 
{
}

void 
Subsystem::walk()
{
    subStatus().initialising();
    initialise();


    if ( !isStopping() )
    {
        subStatus().working();
        work();
    }

    subStatus().finalising();
    finalise();

    subStatus().status().setSubsystemStatus( subStatus().name(), gbxutilacfr::SubsystemShutdown, gbxutilacfr::SubsystemOk );
}
