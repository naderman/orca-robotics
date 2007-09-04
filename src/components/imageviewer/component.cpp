/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "component.h"
#include "mainloop.h"

// implementations of Ice objects
#include "cameraconsumerI.h"


using namespace std;
using namespace imageviewer;

Component::Component()
    : orcaice::Component( "ImageViewer", orcaice::HomeInterface  ),
      dataPipe_(2,orcaice::BufferTypeCircular),
      mainloop_(0)
{
}

Component::~Component()
{
    // do not delete inputLoop_ or mainloop_!!! They derive from Ice::Thread and deletes itself.
}

// NOTE: this function returns after it's done, all variables that need to be permanent must
//       be declared as member variables.
void 
Component::start()
{
    // create a callback object to recieve images
    Ice::ObjectPtr consumer = new CameraConsumerI( dataPipe_ );
    orca::CameraConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::CameraConsumerPrx>( context(), consumer );
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();
    
    //
    // start main loop
    //
    mainloop_ = new MainLoop( callbackPrx, dataPipe_, context() );
    mainloop_->start();
    
    // the rest is handled by the application/service
}

void 
Component::stop()
{
    tracer()->info("stopping component...");
    orcaice::stopAndJoin( mainloop_ );
}
