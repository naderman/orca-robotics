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
#include <orcaice/orcaice.h>

#include "display.h"
#include "mainthread.h"

using namespace std;

namespace systemstatusmon {

MainThread::MainThread( const orcaice::Context &context ) : 
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void 
MainThread::initialise()
{    
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;
    
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    string displayType = orcaice::getPropertyWithDefault( props, prefix+"DisplayType", "colourtext" );
    
    if (displayType=="simpletext")
    {
        display_ = new SimpleDisplay( context_, this );
    }
    else if (displayType=="colourtext")
    {
        display_ = new ColourTextDisplay( context_, this );
    }
    else
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Unknown display type: " + displayType );
    }
}

void
MainThread::work()
{
    
    while ( !isStopping() )
    {
        display_->refresh();
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }
}

}
