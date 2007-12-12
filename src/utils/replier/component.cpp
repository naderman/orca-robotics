/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"
#include "mainthread.h"

using namespace std;
using namespace replier;

Component::Component() :
    orcaice::Component("Replier"),
    mainThread_(0)
{
}

void
Component::start()
{
    //start the main loop and pass it the pointer to the proxy
    mainThread_ = new MainThread( context() );
    mainThread_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 2 );
    hydroutil::stopAndJoin( mainThread_ );
    tracer().debug( "component stopped", 2 );
}
