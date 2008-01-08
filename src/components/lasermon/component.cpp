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

using namespace lasermon;

Component::Component() : 
    orcaice::Component( "LaserMon"  )
{
}

void
Component::start()
{
    consumer_ = new RangeScanner2dConsumerI;
    // the rest is handled by the application/service

    thread_ = new MainThread( consumer_, context() );
    thread_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 5 );
    hydroiceutil::stopAndJoin( thread_ );
    tracer().debug( "stopped component", 5 );
}
