/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    SafeThread( context.tracer() ),
    context_(context)
{
}

void
MainThread::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );
    
    //TODO: make configurable
    string displayType = "colourtext";
    
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
    
    while (!isStopping())
    {
        cout << "Refreshing display now" << endl;
        display_->refresh();
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }
}

}
