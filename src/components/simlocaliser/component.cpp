/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
