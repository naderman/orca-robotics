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

using namespace gps2localise2d;

Component::Component()
    : orcaice::Component( "Gps2Localise2d" )
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
    context().tracer().debug("Stopping component", 2 );
    hydroiceutil::stopAndJoin( mainThread_ );
}
