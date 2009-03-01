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

#include "component.h"
#include "networkthread.h"

#include "term-iostream/termiostreamuser.h"
#ifdef HAVE_TERM_NCURSES_DRIVER   
#   include "term-ncurses/termncursesuser.h"
#endif

using namespace std;
using namespace tracermon;

Component::Component()
    : orcaice::Component( "TracerMon", orcaice::HomeInterface )
{
}

void
Component::start()
{
    Ice::PropertiesPtr props = context().properties();
    std::string prefix = context().tag()+".Config.";

    // which driver to load?
    std::string driverName = orcaice::getPropertyWithDefault( props, prefix+"Driver", "term-iostream" );

    //
    // USER & DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
    User* userDriver = 0;

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
    }
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    NetworkThread* networkThread = new NetworkThread( userDriver, context() );
    netThread_ = (gbxiceutilacfr::SafeThread*)networkThread;
    Network* netDriver = (Network*)networkThread;
    netThread_->start();

    // important: must use 
    userDriver->enable( netDriver );
    usrThread_->start();
}

void
Component::stop()
{
    gbxiceutilacfr::stopAndJoin( netThread_ );

    // userMainThread_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
//     tracer().info( "Component is quitting but the UserMainThread is blocked waiting for user input.");
//     tracer().print( "************************************************" );
//     tracer().print( "Press any key or shake the joystick to continue." );
//     tracer().print( "************************************************" );
    
    gbxiceutilacfr::stopAndJoin( usrThread_ );
}
