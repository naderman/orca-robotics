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

#include "keyboardtermiodriver.h"
#include "asciichar.h"
#include "../network.h"

using namespace std;
using namespace teleop;

void keyboardHelp()
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
};

KeyboardTermioDriver::KeyboardTermioDriver( Network* network, bool enableStepInputs ) :
    network_(network),
    enableStepInputs_(enableStepInputs)
{
}

KeyboardTermioDriver::~KeyboardTermioDriver()
{
}

int 
KeyboardTermioDriver::enable()
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

int 
KeyboardTermioDriver::disable()
{
    // revert to normal input mode
    ioctl( kfd_, TCSETA, &cooked_ );
    //cout<<"keyboarddriver: Cleaned up."<<endl;

    return 0;
}

// read next command from the keyboard
int 
KeyboardTermioDriver::read()
{
    // storage for loop variables
    char c;

    // get the next event from the keyboard
    // note the global scope of this library function
    // (as apposed the member function KeyboardTermioDriver::read() )
    if( ::read(kfd_, &c, 1) < 0 )
    {
        network_->newRelativeCommand( 0.0, 0.0, 0.0 );

        string errString = "failed to read from keyboard";
        throw orcaice::HardwareException( ERROR_INFO, errString );
    }

    switch( c )
    {
        case KEYCODE_i:
            network_->newCommandIncrement( +1, 0, 0 );
            break;
        case KEYCODE_k:
            network_->newCommandIncrement( -1, 0, 0 );
            break;
        case KEYCODE_o:
            network_->newRelativeCommand( 0.0, TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED );
            break;
        case KEYCODE_j:
            network_->newCommandIncrement( 0, 0, +1 );
            break;
        case KEYCODE_l:
            network_->newCommandIncrement( 0, 0, -1 );
            break;
        case KEYCODE_u:
            network_->newRelativeCommand( TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED, 0.0 );
            break;
        case KEYCODE_1:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.1, 0.0, 0.0 );
            break;
        case KEYCODE_2:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.2, 0.0, 0.0 );
            break;
        case KEYCODE_3:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.3, 0.0, 0.0 );
            break;
        case KEYCODE_4:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.4, 0.0, 0.0 );
            break;
        case KEYCODE_5:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.5, 0.0, 0.0 );
            break;
        case KEYCODE_6:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.6, 0.0, 0.0 );
            break;
        case KEYCODE_7:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.7, 0.0, 0.0 );
            break;
        case KEYCODE_8:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.8, 0.0, 0.0 );
            break;
        case KEYCODE_9:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 0.9, 0.0, 0.0 );
            break;
        case KEYCODE_0:
            if ( enableStepInputs_ )
                network_->newRelativeCommand( 1.0, 0.0, 0.0 );
            break;
        default:
            // any other key sends 'stop' command
            network_->newRelativeCommand( 0.0, 0.0, 0.0 );
    }

    return 0;
}
