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

void InputLoop::readConfigs()
{
    //
    // Read settings
    //
    config_.maxSpeed = orcaiceutil::getPropertyAsDoubleWithDefault( current_.properties(),
                "Teleop.Config.MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaiceutil::getPropertyAsDoubleWithDefault( current_.properties(),
                "Teleop.Config.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    string driverName = orcaiceutil::getPropertyWithDefault( current_.properties(), 
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
        current_.logger()->trace("remote",errorStr);
        throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errorStr );
    }
}

// read commands from the keyboard. Launced in a separate thread.
void InputLoop::run()
{
    //cout<<"starting inputloop"<<endl;
    readConfigs();

    // based on the config parameter, create the right driver
    switch ( driverType_ )
    {
        case InputDriver::KEYBOARD_DRIVER :
#ifdef HAVE_KEYBOARD_DRIVER
            current_.logger()->trace("remote","loading keyboard driver");
            driver_ = new TeleopKeyboardDriver( config_ );
#endif
            break;
        case InputDriver::JOYSTICK_DRIVER :
#ifdef HAVE_JOYSTICK_DRIVER
            current_.logger()->trace("remote","loading joystick driver");
            driver_ = new TeleopJoystickDriver( config_ );
#endif
            break;
        case InputDriver::FAKE_DRIVER :
            current_.logger()->trace("remote","loading fake driver");
            driver_ = new TeleopFakeDriver( config_ );
            break;
        case InputDriver::UNKNOWN_DRIVER :
            string errorStr = "Unknown driver type. Cannot talk to hardware.";
            current_.logger()->trace("remote",errorStr);
            throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errorStr );
    }
    // don't forget!
    while ( driver_->enable() ) {
        current_.logger()->trace("remote","failed to enable driver");
        sleep(1);
    }
    current_.logger()->trace("remote","driver enabled");

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
    if ( driver_->disable() ) {
        current_.logger()->trace("remote","failed to disable driver");
    }
    current_.logger()->trace("remote","driver disabled");

}
