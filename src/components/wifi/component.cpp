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
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // only needed for Home and Status interfaces
    // this may throw, but may as well quit right then
    activate();

    //
    // Hardware handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    mainloop_ = new MainLoop( context() );
    mainloop_->start();
    
    // the rest is handled by the application/service

}

void Component::stop()
{
    if ( mainloop_ ) {
        IceUtil::ThreadControl mainLoopControl = mainloop_->getThreadControl();
        tracer()->debug("stopping mainloop", 5 );
        mainloop_->stop();
        tracer()->debug("joining mainloop", 5 );
        mainLoopControl.join();
    }
}

}
