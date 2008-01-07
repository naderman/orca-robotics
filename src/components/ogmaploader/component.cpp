/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>

#include "component.h"
#include "initthread.h"

using namespace std;
using namespace ogmaploader;

Component::Component()
    : orcaice::Component( "OgMapLoader" )
{
}

void
Component::start()
{
    //
    // EXTERNAL PROVIDED INTERFACES
    //
    // create servant for direct connections
    ogMapImpl_ = new orcaifaceimpl::OgMapImpl( "OgMap", context() );

    thread_ = new InitThread( ogMapImpl_, context() );
    thread_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 5 );
    hydroiceutil::stopAndJoin( thread_ );
    tracer().debug( "stopped component", 5 );
}
