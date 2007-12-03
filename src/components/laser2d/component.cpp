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

namespace laser2d {

using namespace std;

Component::Component()
    : orcaice::Component( "Laser2d" )
{
}

void
Component::start()
{
    //
    // create and start the main thread
    //
    mainLoop_ = new MainThread( context() );
    mainLoop_->start();
}

void 
Component::stop()
{
    tracer()->debug("stopping component...",2);
    hydroutil::stopAndJoin( mainLoop_ );
    tracer()->debug("component stopped.",2);
}

} // namespace
