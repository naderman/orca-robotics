/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaice/orcaice.h>

#include "component.h"
#include "mainloop.h"
// implementations of Ice objects
#include "localise2dI.h"

using namespace std;
using namespace orca;
using namespace simlocaliser;

Component::Component()
    : orcaice::Component( "SimLocaliser" ),
      mainLoop_(0)
{
}

Component::~Component()
{
}

void
Component::start()
{
    //
    // EXTERNAL PROVIDED INTERFACE
    //
    // Find IceStorm Topic to which we'll publish
    // NetworkException will kill it, that's what we want.
    orca::Localise2dConsumerPrx localise2dPublisher;
    IceStorm::TopicPrx localiseTopicPrx = orcaice::connectToTopicWithTag<Localise2dConsumerPrx>
        ( context(), localise2dPublisher, "Localise2d" );
    
    // create servant for direct connections
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr localise2dObj = new Localise2dI( localiseTopicPrx, locBuffer_ );
    
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context(), localise2dObj, "Localise2d" );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw, but may as well quit right then
    activate();

    //
    // MAIN DRIVER LOOP
    //
    mainLoop_ = new MainLoop( localise2dPublisher, locBuffer_, context() );
    mainLoop_->start();    
}

void
Component::stop()
{
    orcaice::Thread::stopAndJoin( mainLoop_ );
}
