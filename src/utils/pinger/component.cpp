/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"

using namespace std;
using namespace pinger;

Component::Component() :
    orcaice::Component("Pinger"),
    handler_(0)
{
}

Component::~Component()
{
}

void
Component::start()
{
    //start the main loop and pass it the pointer to the proxy
    handler_ = new Handler( context() );
    handler_->start();
}

void
Component::stop()
{
    tracer()->debug( "stopping component", 2 );
    orcaiceutil::stopAndJoin( handler_ );
    tracer()->debug( "component stopped", 2 );
}
