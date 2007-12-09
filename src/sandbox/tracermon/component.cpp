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
#include "networkhandler.h"

#include "term-iostream/termiostreamuser.h"
#ifdef HAVE_TERM_NCURSES_DRIVER   
#include "term-ncurses/termncursesuser.h"
#endif

using namespace std;
using namespace tracermon;

Component::Component()
    : orcaice::Component( "TracerMon", orcaice::HomeInterface ),
      netHandler_(0),
      usrHandler_(0)
{
}

Component::~Component()
{
    // do not delete networkHandler_ and userHandler_!!! 
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
        tracer()->info( "Loading terminal ncurses driver");
        TermNcursesUser* user = new TermNcursesUser( context() );
        usrHandler_ = (hydroutil::Thread*)user;
        userDriver = (User*)user;
#else
        throw hydroutil::Exception( ERROR_INFO, "Can't instantiate driver type 'term-ncurses' because it was not compiled." );
#endif
    }
    else if ( driverName == "term-iostream" ) 
    {
        tracer()->info( "Loading terminal iostream driver");
        TermIostreamUser* userHandler = new TermIostreamUser( context() );
        usrHandler_ = (hydroutil::Thread*)userHandler;
        userDriver = (User*)userHandler;
    }
    else {
        std::string errorStr = "Unknown driver type." + driverName + " Cannot talk to hardware.";
        tracer()->error( errorStr);
        throw hydroutil::HardwareException( ERROR_INFO, errorStr );
    }
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    NetworkHandler* networkHandler = new NetworkHandler( userDriver, context() );
    netHandler_ = (hydroutil::Thread*)networkHandler;
    Network* netDriver = (Network*)networkHandler;
    netHandler_->start();

    // important: must use 
    userDriver->enable( netDriver );
    usrHandler_->start();
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
    hydroutil::stopAndJoin( netHandler_ );

    // userHandler_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
//     tracer()->info( "Component is quitting but the UserHandler is blocked waiting for user input.");
//     tracer()->print( "************************************************" );
//     tracer()->print( "Press any key or shake the joystick to continue." );
//     tracer()->print( "************************************************" );
    
    hydroutil::stopAndJoin( usrHandler_ );
}
