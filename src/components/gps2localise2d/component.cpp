/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "component.h"
#include "mainloop.h"

using namespace std;
using namespace orca;

namespace gps2localise2d {

Component::Component()
    : orcaice::Component( "Gps2Localise2d" )
{
}

Component::~Component()
{
}

void 
Component::start()
{
    context().tracer().info( "Starting component...",2 );
    
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw, but may as well quit right then
    activate();

    mainLoop_ = new MainLoop( context() );
    mainLoop_->start();
}

void 
Component::stop()
{
    context().tracer().debug("Stopping component", 2 );
    hydroutil::stopAndJoin( mainLoop_ );
}

}
