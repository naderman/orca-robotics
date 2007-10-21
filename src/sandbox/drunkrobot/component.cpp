/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>

#include <orcaice/orcaice.h>

// Include headers for the main loop and components. 

#include "component.h"
#include "mainloop.h"

//Also include headers for position2dI.h

#include "position2dI.h"

using namespace std;
using namespace drunk; 

Component::Component():
    orcaice::Component( "DrunkRobot" ), 
    posBuffer_(100,hydroutil::BufferTypeCircular),
    mainLoop_(0)
{

}

Component::~Component(){

}

void
Component::start(){

    // Get a reasonable geometry. 

    geom_.offset.p.x = 0; 
    geom_.offset.p.y = 0; 
    geom_.offset.o = 0; 

    geom_.size.l = 0.1; 
    geom_.size.w = 0.1;  


    IceStorm::TopicPrx topicPrx = orcaice::connectToTopicWithTag<orca::Position2dConsumerPrx>
        ( context(), position2dConsumer_, "Position2d" );

    //First set up our proxies so other people can talk to us:
    position2dObj_ = new Position2dI( posBuffer_, geom_, topicPrx);
    orcaice::createInterfaceWithTag( context(), position2dObj_, "Position2d" );
  
    // Start the component. 

    activate();

    // then we get the main loop started. 
    mainLoop_ = new MainLoop( posBuffer_, position2dConsumer_);

    mainLoop_->start();

}

void
Component::stop()
{
    cout<<"stopping loop"<<endl;
    // Tell the main loop to stop and wait for it to comply
    hydroutil::stopAndJoin( mainLoop_ );
}
