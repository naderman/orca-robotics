/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include "inputloop.h"

#ifdef HAVE_KEYBOARD_DRIVER
    #include "keyboard/teleopkeyboarddriver.h"
#endif
#ifdef HAVE_JOYSTICK_DRIVER
    #include "joystick/teleopjoystickdriver.h"
#endif
#include "teleopfakedriver.h"

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/exceptions.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;
using namespace orca;

InputLoop::InputLoop( orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> *commands ) :
        commandBuffer_(commands),
        driver_(0),
        driverType_(InputDriver::UNKNOWN_DRIVER)
{
}

InputLoop::~InputLoop()
{
    delete driver_;
}

void InputLoop::setupConfigs( const Ice::PropertiesPtr & properties )
{
    //
    // Read settings
    //
    config_.maxSpeed = orcaiceutil::getPropertyAsDoubleWithDefault( properties,
                "Teleop.Config.MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaiceutil::getPropertyAsDoubleWithDefault( properties,
                "Teleop.Config.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    string driverName = orcaiceutil::getPropertyWithDefault( properties, 
                "Teleop.Config.Driver", "keyboard" );

    if ( driverName == "keyboard" ) {
        driverType_ = InputDriver::KEYBOARD_DRIVER;
    }
    else if ( driverName == "joystick" ) {
        driverType_ = InputDriver::JOYSTICK_DRIVER;
    }
    else if ( driverName == "fake" ) {
        driverType_ = InputDriver::FAKE_DRIVER;
    }
    else {
        driverType_ = InputDriver::UNKNOWN_DRIVER;
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errorStr );
    }

    /*
    xmlDoc->getParam( "useJoystick", use_joystick, DEFAULT_USE_JOYSTICK );
    xmlDoc->getParam( "joystickPort", joystickPort, DEFAULT_JOYSTICK_PORT );
    xmlDoc->getParam( "maxLinSpeed", maxSpeed_, DEFAULT_MAX_SPEED );
    xmlDoc->getAngleParam( "maxAngSpeed", maxTurnrate_, DEFAULT_MAX_TURN );
    xmlDoc->getParam( "timeout", timeout, DEFAULT_TIMEOUT );
    xmlDoc->getParam( "verbose", verbose, DEFAULT_VERBOSE );
    */
}

// read commands from the keyboard. Launced in a separate thread.
void InputLoop::run()
{
    // based on the config parameter, create the right driver
    switch ( driverType_ )
    {
        case InputDriver::KEYBOARD_DRIVER :
#ifdef HAVE_KEYBOARD_DRIVER
            cout<<"loading keyboard driver"<<endl;
            driver_ = new TeleopKeyboardDriver( config_ );
#endif
            break;
        case InputDriver::JOYSTICK_DRIVER :
#ifdef HAVE_JOYSTICK_DRIVER
            cout<<"loading joystick driver"<<endl;
            driver_ = new TeleopJoystickDriver( config_ );
#endif
            break;
        case InputDriver::FAKE_DRIVER :
            driver_ = new TeleopFakeDriver( config_ );
            break;
        case InputDriver::UNKNOWN_DRIVER :
            string errorStr = "Unknown driver type. Cannot talk to hardware.";
            throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errorStr );
    }
    // don't forget!
    driver_->enable();

    Velocity2dCommandPtr currCommand = new Velocity2dCommand;
    Velocity2dCommandPtr lastCommand = new Velocity2dCommand;

    while ( isActive() )
    {
        // remember last command so we can tell if anything has changed
        lastCommand->motion.v.x = currCommand->motion.v.x;
        lastCommand->motion.w = currCommand->motion.w;

        // Read from the input
        driver_->readdata( currCommand );

        //cout<<"current command: "<<currCommand<<endl;

        // commit change only if something has actually changed
        if ( lastCommand->motion.v.x != currCommand->motion.v.x ||
             lastCommand->motion.w != currCommand->motion.w )
        {
            //cout<<"InputLoop::run: pushing new command into buffer"<<endl;
            commandBuffer_->push( currCommand );
        }
    }

    // reset the hardware
    driver_->disable();
}
