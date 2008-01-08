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

using namespace pixmaploader;

Component::Component()
    : orcaice::Component( "PixMapLoader" )
{
}

void
Component::start()
{
    //
    // EXTERNAL PROVIDED INTERFACES
    //
    // create servant for direct connections
    pixMapImpl_ = new orcaifaceimpl::PixMapImpl( "PixMap", context() );

    thread_ = new MainThread( pixMapImpl_, context() );
    thread_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 5 );
    hydroiceutil::stopAndJoin( thread_ );
    tracer().debug( "stopped component", 5 );
}
