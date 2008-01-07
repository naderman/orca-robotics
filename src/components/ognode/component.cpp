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

using namespace ognode;

Component::Component()
    : orcaice::Component( "OgNode", orcaice::HomeInterface  )
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
    hydroiceutil::stopAndJoin( mainThread_ );
}
