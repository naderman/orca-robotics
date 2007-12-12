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

namespace wifi {

Component::Component()
    : orcaice::Component( "Wifi" )
{
}

void
Component::start()
{
    mainThread_ = new MainThread( context() );
    mainThread_->start();
}

void Component::stop()
{    
    tracer().debug( "Stopping component", 2 );
    // alexm: why is this?
//    hydroutil::stopAndJoin( mainThread_ );
    tracer().info( "Component stopped" );
}

}
