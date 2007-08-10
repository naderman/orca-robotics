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
#include "odometry2dconsumerI.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;

Component::Component()
    : orcaice::Component( "FaithLocaliser" ),
      mainLoop_(0)
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
    // this may throw, but may as well quit right then
    activate();

    //
    // MAIN DRIVER LOOP
    //
    
    mainLoop_ = new MainLoop( context() );
    mainLoop_->start();
}

void
Component::stop()
{
    tracer()->debug( "stopping component", 5 );
    orcaice::stopAndJoin( mainLoop_ );
    tracer()->debug( "stopped component", 5 );
}
