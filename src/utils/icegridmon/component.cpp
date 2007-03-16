/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "mainloop.h"
#include "component.h"

using namespace std;

namespace icegridmon {

Component::Component( const std::string & compName )
    : orcaice::Component( compName ),
      mainLoop_(0)
{
}

Component::~Component()
{
}

void 
Component::start()
{
    mainLoop_ = new MainLoop( context() );
    mainLoop_->start();
}

void 
Component::stop()
{
    tracer()->debug("stopping component...",2);
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

} // namespace
