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
#include <orcaice/proputils.h>
#include <orcaobj/mathdefs.h>
#include <orcaice/exceptions.h>

#include "keyboardtermiodriver.h"
#include "asciichar.h"

using namespace std;
using namespace orca;
using namespace teleop;


KeyboardTermioDriver::KeyboardTermioDriver( const InputDriver::Config &cfg ) :
        config_(cfg)
{
    // init internal data storage
    command_.motion.v.x = 0.0;
    command_.motion.v.y = 0.0;
    command_.motion.w = 0.0;

    // one key stroke changes commands by these values
    deltaSpeed_ = 0.05;     // [m/s]
    deltaTurnrate_ = DEG2RAD(2.0);  // [rad/sec]
}

KeyboardTermioDriver::~KeyboardTermioDriver()
{
}

int KeyboardTermioDriver::enable()
{
    kfd_ = 0;

    // ref: on termio(s) stuff see
    // http://publib16.boulder.ibm.com/pseries/en_US/files/aixfiles/termios.h.htm
    struct termio raw;
    // get current (not-raw) console settings
    ioctl( kfd_, TCGETA, &cooked_ );
    // make a copy so we can restore the original settings
    memcpy(&raw, &cooked_, sizeof(struct termio));

    // turn off echo to screen and erase functions
    raw.c_lflag &=~ (ICANON | ECHO);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    // put the console in raw mode
    ioctl( kfd_, TCSETA, &raw );

    // display keymap
    keyboardHelp();

    //cout<<"enabled."<<endl;
    return 0;
}

int KeyboardTermioDriver::disable()
{
    // revert to normal input mode
    ioctl( kfd_, TCSETA, &cooked_ );
    //cout<<"keyboarddriver: Cleaned up."<<endl;

    return 0;
}

// read next command from the keyboard
int KeyboardTermioDriver::read( orca::Velocity2dCommand& data )
{
    // storage for loop variables
    char c;

    // get the next event from the keyboard
    // note the global scope of this library function
    // (as apposed the member function KeyboardTermioDriver::read() )
    if( ::read(kfd_, &c, 1) < 0 )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;

        string errString = "failed to read from keyboard";
        throw orcaice::HardwareException( ERROR_INFO, errString );
    }

    switch( c )
    {
        case KEYCODE_i:
            command_.motion.v.x += deltaSpeed_;
            break;
        case KEYCODE_k:
            command_.motion.v.x -= deltaSpeed_;
            break;
        case KEYCODE_o:
            command_.motion.v.x = 0.0;
            break;
        case KEYCODE_j:
            command_.motion.w += deltaTurnrate_;
            break;
        case KEYCODE_l:
            command_.motion.w -= deltaTurnrate_;
            break;
        case KEYCODE_u:
            command_.motion.w = 0.0;
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
        default:
            // any other key sends 'stop' command
            command_.motion.v.x = 0.0;
            command_.motion.v.y = 0.0;
            command_.motion.w = 0.0;
    }

    // apply max limits 
    if ( fabs(command_.motion.v.x) > config_.maxSpeed ) {
        command_.motion.v.x =
                (command_.motion.v.x / fabs(command_.motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(command_.motion.w) > config_.maxTurnrate ) {
        command_.motion.w =
                (command_.motion.w / fabs(command_.motion.w)) * config_.maxTurnrate;
    }

    // return updated command
    // ice_clone() doesn't work for some reason
    //data = command_.ice_clone();
    //data.timeStamp = ?;
    data.motion.v.x = command_.motion.v.x;
    data.motion.v.y = command_.motion.v.y;
    data.motion.w = command_.motion.w;

    return 0;
}


void KeyboardTermioDriver::keyboardHelp()
{
    puts("-----------------------------------------");
    puts("Moving around:");
    puts("i : increment speed");
    puts("k : decrement speed");
    puts("j : increment turnrate (positive to the left)");
    puts("l : decrement turnrate (positive to the left)");
    puts("u : stop turn");
    puts("o : stop movement (but still turn)");
//    puts("q/z : increase/decrease speed by 10%");
//    puts("w/x : increase/decrease only pos speed by 10%");
//    puts("e/c : increase/decrease only turn speed by 10%");
    puts("anything else : stop");
    puts("-----------------------------------------");
}
