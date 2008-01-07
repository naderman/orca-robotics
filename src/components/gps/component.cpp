/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "component.h"
#include "mainthread.h"

namespace gps {

Component::Component()
    : orcaice::Component( "Gps" )
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
    tracer().debug("stopping component...",2);
    hydroiceutil::stopAndJoin( mainThread_ );
    tracer().debug("component stopped.",2);
}

} // namespace
