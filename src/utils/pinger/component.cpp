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

using namespace pinger;

Component::Component() :
    orcaice::Component("Pinger")
{
}

void
Component::start()
{
    thread_ = new MainThread( context() );
    thread_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 2 );
    hydroiceutil::stopAndJoin( thread_ );
    tracer().debug( "component stopped", 2 );
}
