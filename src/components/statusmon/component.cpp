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

#include "component.h"
#include "mainthread.h"

using namespace std;

namespace statusmon {

Component::Component()
    : orcaice::Component( "StatusMon", orcaice::HomeInterface )
{
}

void
Component::start()
{
    Ice::PropertiesPtr props = context().properties();
    std::string prefix = context().tag()+".Config.";

    // which driver to load?
//     std::string driverName = orcaice::getPropertyWithDefault( props, prefix+"Driver", "term-iostream" );

    //
    // USER & DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
/*    User* userDriver = 0;

    if ( driverName == "term-ncurses" ) 
    {
#ifdef HAVE_TERM_NCURSES_DRIVER        
        context().tracer().info( "Loading terminal ncurses driver");
        TermNcursesUser* user = new TermNcursesUser( context() );
        usrThread_ = (gbxiceutilacfr::SafeThread*)user;
        userDriver = (User*)user;
#else
        throw gbxutilacfr::Exception( ERROR_INFO, "Can't instantiate driver type 'term-ncurses' because it was not compiled." );
#endif
    }
    else if ( driverName == "term-iostream" ) 
    {
        context().tracer().info( "Loading terminal iostream driver");
        TermIostreamUser* userMainThread = new TermIostreamUser( context() );
        usrThread_ = (gbxiceutilacfr::SafeThread*)userMainThread;
        userDriver = (User*)userMainThread;
    }
    else {
        std::string errorStr = "Unknown driver type." + driverName + " Cannot talk to hardware.";
        context().tracer().error( errorStr);
        throw gbxutilacfr::HardwareException( ERROR_INFO, errorStr );
    }*/
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    MainThread* mainThread = new MainThread( context() );
    mainThread_ = (gbxiceutilacfr::SafeThread*)mainThread;
//     Network* netDriver = (Network*)networkMainThread;
    mainThread_->start();

    // important: must use 
//     userDriver->enable( netDriver );
//     usrThread_->start();
}

void
Component::stop()
{
    gbxiceutilacfr::stopAndJoin( mainThread_ );
}
}
