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

#include "keyboarddriver.h"

//#if HAVE_TERMIO_H
#include <termio.h>
#define KEYBOARD_SUPPORT 1
//#else
//#define KEYBOARD_SUPPORT 0
//#endif

//#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
//#endif

//#if HAVE_FCNTL_H
#include <fcntl.h>
//#endif

#include "asciichar.h"

#include <orcaiceutil/proputils.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;
using namespace orca;

KeyboardDriver::KeyboardDriver( orcaiceutil::PtrProxy* commands ) :
        commandProxy_(commands)
{
    // init internal data storage
    command_ = new Velocity2dCommand;
    command_->twist.velocity.x = 0.0;
    command_->twist.velocity.y = 0.0;
    command_->twist.w = 0.0;
}

KeyboardDriver::~KeyboardDriver()
{
}

void KeyboardDriver::setup( const Ice::PropertiesPtr & properties )
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

void KeyboardDriver::activate()
{
    cout<<"activating device"<<endl;
    start();
}

void KeyboardDriver::deactivate()
{
    cout<<"deactivating device"<<endl;
    stop();
}

// read commands from the keyboard. Launced in a separate thread.
void KeyboardDriver::run()
{
    cout<<"KeyboardDriver::run: starting nicely"<<endl;

    // one key stroke changes commands by these values
    const double deltaSpeed = 0.05;     // [m/s]
    const double deltaTurnrate = DEG2RAD(2.0);  // [rad/sec]

    int kfd = 0;
    // ref: on termio(s) stuff see
    // http://publib16.boulder.ibm.com/pseries/en_US/files/aixfiles/termios.h.htm
    struct termio cooked, raw;
    // get current (not-raw) console settings
    ioctl( kfd, TCGETA, &cooked );
    // make a copy so we can restore the original settings
    memcpy(&raw, &cooked, sizeof(struct termio));
    // turn off echo to screen and erase functions
    raw.c_lflag &=~ (ICANON | ECHO);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    // put the console in raw mode
    ioctl( kfd, TCSETA, &raw );

    // display keymap
    keyboardHelp();

    // storage for loop variables
    char c;
    Velocity2dCommandPtr lastCommand = new Velocity2dCommand;

    while ( isActive() )
    {
        // remember last command so we can tell if anything has changed
        lastCommand->twist.velocity.x = command_->twist.velocity.x;
        lastCommand->twist.w = command_->twist.w;
    
        // get the next event from the keyboard
        if( read(kfd, &c, 1) < 0 )
        {
            cerr << "ERROR(" << __FILE__ << ":" << __LINE__ << "): failed to read keyboard" << endl;
            exit(-1);
        }
        //cout << "TRACE(" << __FILE__ << ":" << __LINE__ << "): parsing keyboard input." << endl;

        switch( c )
        {
            case KEYCODE_i:
                command_->twist.velocity.x += deltaSpeed;
                break;
            case KEYCODE_k:
                command_->twist.velocity.x -= deltaSpeed;
                break;
            case KEYCODE_o:
                command_->twist.velocity.x = 0.0;
                break;
            case KEYCODE_j:
                command_->twist.w += deltaTurnrate;
                break;
            case KEYCODE_l:
                command_->twist.w -= deltaTurnrate;
                break;
            case KEYCODE_u:
                command_->twist.w = 0.0;
                break;
            case KEYCODE_q:
                //speed *= 1.1;
                //turn *= 1.1;
                break;
            case KEYCODE_z:
                //speed *= 0.9;
                //turn *= 0.9;
                break;
            case KEYCODE_w:
                //speed *= 1.1;
                break;
            case KEYCODE_x:
                //speed *= 0.9;
                break;
            case KEYCODE_e:
                //turn *= 1.1;
                break;
            case KEYCODE_c:
                //turn *= 0.9;
                break;
            case KEYCODE_ESCAPE:
                cout<<"quitting on demand"<<endl;
                deactivate();
                break;
            default:
                // any other key sends 'stop' command
                command_->twist.velocity.x = 0.0;
                command_->twist.w = 0.0;
        }

        // apply max limits
        if ( fabs(command_->twist.velocity.x) > maxSpeed_ ) {
            command_->twist.velocity.x =
                (command_->twist.velocity.x / fabs(command_->twist.velocity.x)) * maxSpeed_;
        }
        if ( fabs(command_->twist.w) > maxTurnrate_ ) {
            command_->twist.w =
                (command_->twist.w / fabs(command_->twist.w)) * maxTurnrate_;
        }
        //cout<<"current command: "<<command_<<endl;

        // commit change only if something has actually changed
        if ( lastCommand->twist.velocity.x != command_->twist.velocity.x ||
             lastCommand->twist.w != command_->twist.w )
        {
            commandProxy_->set( command_ );
        }
    }

    // revert to normal input mode
    ioctl( kfd, TCSETA, &cooked );

    cout<<"Cleaned up, shutting down the communicator..."<<endl;
    try {
        comm_->shutdown();
    }
    catch ( const Ice::CommunicatorDestroyedException & e ) {
        // it's ok, if this is a Ctrl-C event, the communicator is already destroyed
    }
}

void KeyboardDriver::keyboardHelp()
{
    puts("-----------------------------------------");
    puts("Moving around:");
    puts("i : forward");
    puts("j : left");
    puts("l : right");
    puts("k : backward");
    puts("u : stop turn");
    puts("o : stop movement (but still turn)");
    puts("q/z : increase/decrease speed by 10%");
    puts("w/x : increase/decrease only pos speed by 10%");
    puts("e/c : increase/decrease only turn speed by 10%");
    puts("anything else : stop");
    puts("-----------------------------------------");
}
