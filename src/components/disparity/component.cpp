/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "mainthread.h"

namespace disparity
{

Component::Component()
: orcaice::Component( "Disparity" )
{
}

Component::~Component()
{
}

void
Component::start()
{
    // create mainthread, which will own everything
    IceUtil::Handle<MainThread> threadPtr = new MainThread( context() );
    mainThread_ = threadPtr;
       
    // start thread
    mainThread_->start();

    // give MainThread a chance to write something to the history
    bool forceHistory = false;
    context().history().autoStart( forceHistory );
    context().history().flush();

    // if the plugin requires an event loop, run it and handle shutdowns properly
    if( threadPtr->hasPluginEventLoop() )
    {
        threadPtr->executePluginEventLoop();

        context().communicator()->shutdown();
    }
}

void
Component::stop()
{
    context().tracer().debug( "stopping main thread", 5 );
    gbxiceutilacfr::stopAndJoin( mainThread_ );
    context().tracer().debug( "stopped main thread", 5 );

    // cause mainThread to be destroyed
    mainThread_ = 0;

    bool forceHistory = false;
    context().history().autoFinish( forceHistory );
    context().history().flush();
}

} // namespace

