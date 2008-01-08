/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "mainthread.h"

namespace pingserver {

Component::Component()
    : orcaice::Component( "PingServer" )
{
}

void
Component::start()
{
    pingerInterface_ = new PingerI( "Pinger", context() );

    thread_ = new MainThread( pingerInterface_, context() );
    thread_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 5 );
    hydroiceutil::stopAndJoin( thread_ );
    tracer().debug( "stopped component", 5 );
}

}
