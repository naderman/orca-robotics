/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>

#include "component.h"
#include "mainloop.h"

using namespace std;
using namespace orca;

namespace wifi {

Component::Component()
    : orcaice::Component( "Wifi" )
{
}

Component::~Component()
{
}

void
Component::start()
{
    tracer()->info( "Starting component" );
    
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // only needed for Home and Status interfaces
    // this may throw, but may as well quit right then
    activate();

    //
    // Hardware handling loop
    //
    mainloop_ = new MainLoop( context() );
    mainloop_->start();
    
    tracer()->debug( "Component started", 2 );
    
    // the rest is handled by the application/service

}

void Component::stop()
{    
    tracer()->debug( "Stopping component", 2 );
//    orcaice::stopAndJoin( mainloop_ );
    tracer()->info( "Component stopped" );
}

}
