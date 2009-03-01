/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "component.h"
#include <orcateleop/networkthread.h>
#include <orcateleop/termdisplaythread.h>
#include <orcateleop/inputthread.h>

using namespace std;

namespace teleop {

Component::Component() :
    orcaice::Component( "Teleop", orcaice::NoStandardInterfaces ),
    networkThread_(0),
    displayThread_(0),
    inputThread_(0)
{
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    context().tracer().debug("starting component",2);

    //
    // USER DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
    displayThread_ = new orcateleop::TermDisplayThread( context() );
    displayThread_->start();
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    networkThread_ = new orcateleop::NetworkThread( (orcateleop::Display*)displayThread_, context() );
    networkThread_->start();

    //
    // USER INPUT
    //
    // the constructor may throw, we'll let the application shut us down
    inputThread_ = new orcateleop::InputThread( (orcateleop::Network*)networkThread_, context() );
    inputThread_->start();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    gbxiceutilacfr::stopAndJoin( inputThread_ );
    gbxiceutilacfr::stopAndJoin( networkThread_ );
    gbxiceutilacfr::stopAndJoin( displayThread_ );
}

}
