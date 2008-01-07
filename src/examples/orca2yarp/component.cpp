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

using namespace orca2yarp;

Component::Component() : 
    orcaice::Component( "Orca2Yarp" )
{
}

void
Component::start()
{
    //start the main loop
    thread_ = new MainThread( context() );
    thread_->start();
    // the rest is handled by the application/service
}

void
Component::stop()
{
    hydroiceutil::stopAndJoin( thread_ );
}
