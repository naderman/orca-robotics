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
#include <iostream>
#include "component.h"


using namespace std;

namespace pingserver {

Component::Component()
    : orcaice::Component( "PingServer" )
{
}

// NOTE: this function returns after it's done, all variable that need to be permanent must
//       be declared as member variables.
void
Component::start()
{
    context().tracer()->info( "Component::start()" );

    //
    // ENABLE NETWORK
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    pingerInterface_ = new PingerI( "Pinger", context() );
    pingerInterface_->initInterface();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    // nothing to do
}

}
