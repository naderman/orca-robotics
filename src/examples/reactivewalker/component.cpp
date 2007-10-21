/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "algothread.h"

using namespace reactivewalker;

Component::Component()
    : orcaice::Component( "ReactiveWalker" ),
      algoThread_(0)
{
}

Component::~Component()
{
    // do not delete handlers!!! They derives from Ice::Thread and delete themselves.
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void Component::start()
{
    //
    // The thread which will handle the algorithm
    //
    algoThread_ = new AlgoThread( context() );
    algoThread_->start();
    
    // the rest is handled by the application/service
}

void Component::stop()
{
    tracer()->debug("stopping component...",2);
    hydroutil::stopAndJoin( algoThread_ );
    tracer()->debug("component stopped.",2);
}
