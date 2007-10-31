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
#include "algohandler.h"

using namespace pathplanner;

Component::Component()
    : orcaice::Component( "PathPlanner" ),
      algoHandler_(0)
{
}

Component::~Component()
{
    // do not delete handlers!!! They derives from Ice::Thread and delete themselves.
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void 
Component::start()
{
    context().tracer()->info( "Component starting." );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // only needed for Home and Status interfaces
    // this may throw, but may as well quit right then
    activate();

    //
    // Hardware handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    algoHandler_ = new AlgoHandler( context() );
    algoHandler_->start();
    
    // the rest is handled by the application/service
}

void 
Component::stop()
{
    tracer()->debug( "stopping component", 5 );
    hydroutil::stopAndJoin( algoHandler_ );
    tracer()->debug( "stopped component", 5 );
}
