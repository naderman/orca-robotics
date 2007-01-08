/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "inputhandler.h"
#include "network.h"
#ifdef HAVE_KEYBOARD_TERMIO_DRIVER
    #include "kbd-termio/keyboardtermiodriver.h"
#endif
#ifdef HAVE_JOYSTICK_DRIVER
    #include "joystick/joystickdriver.h"
#endif

using namespace std;
using namespace teleop;

InputHandler::InputHandler( Network* network, const orcaice::Context& context ) :
    network_(network),
    driver_(0),
    context_(context)
{
}

InputHandler::~InputHandler()
{
    delete driver_;
}

void
InputHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( prop, prefix+"InputDriver", "keyboard" );
    if ( driverName == "keyboard" )
    {
#ifdef HAVE_KEYBOARD_TERMIO_DRIVER
        context_.tracer()->info("loading 'keyboard' driver");
        driver_ = new KeyboardTermioDriver( network_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'keyboard' because it was not built!" );
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
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'keyboard', 'joystick'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }    

    // don't forget to enable the driver, but check isActive() to see if we should quit
    while ( driver_->enable() && isActive() ) {
        context_.tracer()->warning("Failed to enable input driver. Will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // check again to make sure we are not being terminated
    if ( !isActive() ) {
        return;
    }
    context_.tracer()->debug("Input driver enabled",2);

    //
    // Main loop
    //
    while ( isActive() )
    {
        driver_->read();
    } // end of main loop

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("Failed to disable input driver");
    }
    context_.tracer()->debug("Input driver disabled",2);

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        stringstream ss;
        ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        stringstream ss;
        ss << "unexpected std exception (possibly OrcaIce): " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "InputHandler: stopped.",5 );
}