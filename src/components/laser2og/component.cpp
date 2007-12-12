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
#include "handler.h"

using namespace laser2og;

Component::Component()
    : orcaice::Component( "Laser2Og" ),
      handler_(0)
{
}

Component::~Component()
{
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void
Component::start()
{
    activate();
    handler_ = new Handler( context() );
    handler_->start();
}

void
Component::stop()
{
    tracer().debug( "stopping component", 5 );
    hydroutil::stopAndJoin( handler_ );
    tracer().debug( "stopped component", 5 );
}
