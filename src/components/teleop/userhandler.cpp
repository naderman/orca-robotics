/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "userhandler.h"

#ifdef HAVE_KEYBOARD_TERMIO_DRIVER
    #include "kbd-termio/keyboardtermiodriver.h"
#endif
#ifdef HAVE_KEYBOARD_NCURSES_DRIVER
    #include "kbd-ncurses/keyboardncursesdriver.h"
#endif
#ifdef HAVE_JOYSTICK_DRIVER
    #include "joystick/joystickdriver.h"
#endif
#include "fakedriver.h"

#include "stdoutdisplayhandler.h"

using namespace std;
using namespace orca;
using namespace teleop;

UserHandler::UserHandler( orcaice::Buffer<orca::Velocity2dCommand> *commands,
                    const orcaice::Context & context )
      : commandPipe_(commands),
        driver_(0),
        displayHandler_(0),
        context_(context)
{
    // this is the last place we can throw exceptions from.
    // after this the thread will be launched
    init();
}

UserHandler::~UserHandler()
{
    delete driver_;
#ifndef HAVE_KEYBOARD_NCURSES_DRIVER
    // this the only driver which has an intergrated display driver,
    // so it does not need to be deleted separately
    delete displayHandler_;
#endif
}

void
UserHandler::init()
{
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();;
    std::string prefix = context_.tag() + ".Config.";
    
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxTurnrate", 40.0 )*DEG2RAD_RATIO;

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "kbd-termio" );
    if ( driverName == "kbd-termio" )
    {
#ifdef HAVE_KEYBOARD_TERMIO_DRIVER
        context_.tracer()->info("loading 'kbd-termio' driver");
        driver_ = new KeyboardTermioDriver( config_ );
        displayHandler_ = new StdoutDisplayHandler();
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'kbd-termio' because it was not built!" );
#endif
    }
    else if ( driverName == "kbd-ncurses" )
    {
#ifdef HAVE_KEYBOARD_NCURSES_DRIVER
        context_.tracer()->info("loading 'kbd-ncurses' driver");
        KeyboardNcurcesDriver* keyboardTermioDriver = new KeyboardNcurcesDriver( config_ );
        // this driver implements both interfaces
        driver_ = keyboardTermioDriver;
        displayHandler_ = keyboardTermioDriver;
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'kbd-ncurses' because it was not built!" );
#endif
    }
    else if ( driverName == "joystick" )
    {
#ifdef HAVE_JOYSTICK_DRIVER
        context_.tracer()->info("loading 'joystick' driver");
        
        std::string joystickPrefix = prefix + "Joystick.";
        config_.joystickDevice = orcaice::getPropertyWithDefault( prop,
                joystickPrefix+"Device", "auto" );
        driver_ = new JoystickDriver( config_ );
        displayHandler_ = new StdoutDisplayHandler();
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'joystick' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->info("loading 'fake' driver");
        driver_ = new FakeDriver( config_ );
        displayHandler_ = new StdoutDisplayHandler();
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'kbd-termio', 'kbd-ncurses', 'joystick', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }
}

// read commands from the keyboard. Launced in a separate thread.
void
UserHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // don't forget to enable the driver, but check isActive() to see if we should quit
    while ( driver_->enable() && isActive() ) {
        context_.tracer()->warning("Failed to enable driver. Will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // check again to make sure we are not being terminated
    if ( !isActive() ) {
        return;
    }
    
    context_.tracer()->debug("driver enabled",5);
    
    Velocity2dCommand currCommand;
    Velocity2dCommand lastCommand;
    
    while ( isActive() )
    {
        // remember last command so we can tell if anything has changed
        lastCommand.motion.v.x = currCommand.motion.v.x;
        lastCommand.motion.w = currCommand.motion.w;

        //
        // Read user input
        //
        driver_->read( currCommand );

        // commit change only if something has actually changed
        if ( lastCommand.motion.v.x != currCommand.motion.v.x ||
             lastCommand.motion.w != currCommand.motion.w )
        {
            commandPipe_->push( currCommand );
        }
    }

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("failed to disable driver");
    }
    context_.tracer()->debug("driver disabled");

    } // try
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        context_.communicator()->destroy();
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "UserHandler: stopped.",5 );
}
