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
#include "mainthread.h"

using namespace std;
using namespace simlocaliser;

Component::Component()
    : orcaice::Component( "SimLocaliser" )
{
}

void
Component::start()
{
    //
    // MAIN DRIVER LOOP
    //
    thread_ = new MainThread( context() );
    thread_->start();    
}

void
Component::stop()
{
    hydroiceutil::stopAndJoin( thread_ );
}
