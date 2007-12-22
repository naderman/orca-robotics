/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"
#include "mainthread.h"

using namespace brick;

Component::Component() :
    orcaice::Component("Brick")
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
    hydroutil::stopAndJoin( mainThread_ );
}
