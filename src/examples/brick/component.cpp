/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"
#include "mainthread.h"

using namespace std;
using namespace brick;

Component::Component() :
    orcaice::Component("Brick"),
    mainThread_(0)
{
}

Component::~Component()
{
}

void
Component::start()
{
    tracer()->debug( "Starting component", 2 );

    //start the main loop and pass it the pointer to the proxy
    mainThread_ = new MainThread( context() );
    mainThread_->start();

    tracer()->debug( "Component started", 2 );
}

void
Component::stop()
{
    tracer()->debug( "Stopping component", 2 );
    orcaiceutil::stopAndJoin( mainThread_ );
    tracer()->debug( "Component stopped", 2 );
}
