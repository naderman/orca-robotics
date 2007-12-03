/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "mainthread.h"

using namespace std;
using namespace faithlocaliser;

Component::Component()
    : orcaice::Component( "FaithLocaliser" )
{
}

void
Component::start()
{
    mainThread_ = new MainThread( context() );
    mainThread_->start();
}

void
Component::stop()
{
    tracer()->debug( "stopping component", 5 );
    hydroutil::stopAndJoin( mainThread_ );
    tracer()->debug( "stopped component", 5 );
}
