/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include "term-ncurses/termncursesuser.h"
#endif

using namespace std;
using namespace tracermon;

Component::Component()
    : orcaice::Component( "TracerMon", orcaice::HomeInterface ),
      netMainThread_(0),
      usrMainThread_(0)
{
}

Component::~Component()
{
    // do not delete networkMainThread_ and userMainThread_!!! 
    // They derive from Ice::Thread and delete itself.
}

void
Component::start()
{
    Ice::PropertiesPtr props = properties();
    std::string prefix = tag()+".Config.";

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
        tracer().info( "Loading terminal ncurses driver");
        TermNcursesUser* user = new TermNcursesUser( context() );
        usrMainThread_ = (hydroutil::SafeThread*)user;
        userDriver = (User*)user;
#else
        throw hydroutil::Exception( ERROR_INFO, "Can't instantiate driver type 'term-ncurses' because it was not compiled." );
#endif
    }
    else if ( driverName == "term-iostream" ) 
    {
        tracer().info( "Loading terminal iostream driver");
        TermIostreamUser* userMainThread = new TermIostreamUser( context() );
        usrMainThread_ = (hydroutil::SafeThread*)userMainThread;
        userDriver = (User*)userMainThread;
    }
    else {
        std::string errorStr = "Unknown driver type." + driverName + " Cannot talk to hardware.";
        tracer().error( errorStr);
        throw hydroutil::HardwareException( ERROR_INFO, errorStr );
    }
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    MainThread* networkMainThread = new MainThread( userDriver, context() );
    netMainThread_ = (hydroutil::SafeThread*)networkMainThread;
    Network* netDriver = (Network*)networkMainThread;
    netMainThread_->start();

    // important: must use 
    userDriver->enable( netDriver );
    usrMainThread_->start();
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
    hydroutil::stopAndJoin( netMainThread_ );

    // userMainThread_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
//     tracer().info( "Component is quitting but the UserMainThread is blocked waiting for user input.");
//     tracer().print( "************************************************" );
//     tracer().print( "Press any key or shake the joystick to continue." );
//     tracer().print( "************************************************" );
    
    hydroutil::stopAndJoin( usrMainThread_ );
}
