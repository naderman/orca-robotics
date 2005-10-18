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
#include "teleopfsm.h"

#include "keyboard/keyboarddriver.h"
//#include "joystick/joystickddriver.h"

#include <orcaiceutil/proputils.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;
using namespace orca;

InputLoop::InputLoop( TeleopFsm* fsm, orcaiceutil::PtrBuffer* commands ) :
        fsm_(fsm), commandBuffer_(commands), driver_(0)
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
    maxSpeed_ = orcaiceutil::getPropertyAsDoubleWithDefault( properties, "Teleop.MaxSpeed", 1.0 );
    maxTurnrate_ = orcaiceutil::getPropertyAsDoubleWithDefault( properties, "Teleop.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    cout<<"properties: maxspeed="<<maxSpeed_<<", maxturn="<<maxTurnrate_<<endl;

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
    cout<<"InputLoop::run: starting nicely"<<endl;

    // based on some config parameter, create the right driver
    if ( 1 )
    {
        driver_ = new KeyboardDriver();
    }

    Velocity2dCommandPtr currCommand = new Velocity2dCommand;
    Velocity2dCommandPtr lastCommand = new Velocity2dCommand;

    while ( isActive() )
    {
        // remember last command so we can tell if anything has changed
        lastCommand->motion.v.x = currCommand->motion.v.x;
        lastCommand->motion.w = currCommand->motion.w;

        // Read from the input
        cout<<"InputLoop::run: reading from driver"<<endl;
        driver_->readdata( currCommand );

        // apply max limits
        if ( fabs(currCommand->motion.v.x) > maxSpeed_ ) {
            currCommand->motion.v.x =
                (currCommand->motion.v.x / fabs(currCommand->motion.v.x)) * maxSpeed_;
        }
        if ( fabs(currCommand->motion.w) > maxTurnrate_ ) {
            currCommand->motion.w =
                (currCommand->motion.w / fabs(currCommand->motion.w)) * maxTurnrate_;
        }
        //cout<<"current command: "<<currCommand<<endl;

        // commit change only if something has actually changed
        if ( lastCommand->motion.v.x != currCommand->motion.v.x ||
             lastCommand->motion.w != currCommand->motion.w )
        {
            cout<<"InputLoop::run: pushing new command into buffer"<<endl;
            commandBuffer_->push( currCommand );
        }
    }

    // reset the hardware
    driver_->disable();
}
