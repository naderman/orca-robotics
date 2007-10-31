/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "component.h"
#include "mainloop.h"

using namespace std;
using namespace orca;
using namespace laserfeatures;

Component::Component()
    : orcaice::Component( "LaserFeatureExtractor" ),
      featureInterface_(0),
      mainLoop_(0)
{
}

Component::~Component()
{
    // do not delete mainLoop_!!! They derive from Ice::Thread and self-destruct.
}

// warning: this function returns after it's done, all variable that need to be permanent must
//          be declared as member variables.
void 
Component::start()
{
    context().tracer()->info( "Starting component...",2 );

    //
    // Instantiate External Interface
    //
    featureInterface_ = new orcaifaceimpl::PolarFeature2dImpl( "PolarFeature2d", context() );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw, but may as well quit right then
    activate();

    mainLoop_ = new MainLoop( *featureInterface_, context() );
    mainLoop_->start();
}

void 
Component::stop()
{
    context().tracer()->debug("Stopping component", 2 );
    hydroutil::stopAndJoin( mainLoop_ );
}
