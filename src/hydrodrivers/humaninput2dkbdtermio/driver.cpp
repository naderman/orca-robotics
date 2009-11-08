/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <cstring>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>

#include "driver.h"
#include "asciichar.h"

using namespace std;

namespace humaninput2dkbdtermio
{

namespace {

    // Warning: cut-n-pasted from other drivers.  Is there a good
    // central place to put this function?
    bool dataAvailable( int fd )
    {
        fd_set rfds;
        struct timeval tv;
        int retval;

        // watch the fd to see when it has input
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        // wait up to 100ms
        tv.tv_sec=0;
        tv.tv_usec=100000;

        retval = select(1,&rfds,NULL,NULL,&tv);
        // Don't rely on the value of tv now!

        if ( retval == -1 )
        {
            perror("select()");
            return false;
        }
        else if ( retval )
            return true;
        else
            return false;
    }
}

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

Driver::Driver( const hydroutil::Context &context ) :
    context_(context)
{
    // read driver-specific properties
    std::string prefix = "KbdTermio.";

    enableStepInputs_ = context_.properties().getPropertyAsIntWithDefault( prefix+"StepInputs", 0 );
}

Driver::~Driver()
{
    disable();
}

int 
Driver::enable()
{
    kfd_ = 0;

    // ref: on termio(s) stuff see
    // http://publib16.boulder.ibm.com/pseries/en_US/files/aixfiles/termios.h.htm
    struct termios raw;
    // get current (not-raw) console settings
    tcgetattr( kfd_, &cooked_ );
    // make a copy so we can restore the original settings
    memcpy(&raw, &cooked_, sizeof(struct termios));

    // turn off echo to screen and erase functions
    raw.c_lflag &=~ (ICANON | ECHO);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    // put the console in raw mode
    tcsetattr( kfd_, TCSANOW, &raw );

    // display keymap
    keyboardHelp();

    //cout<<"enabled."<<endl;
    return 0;
}

int 
Driver::disable()
{
    // revert to normal input mode
    tcsetattr( kfd_, TCSANOW, &cooked_ );
    // cout<<"keyboarddriver: Cleaned up."<<endl;

    return 0;
}

// read next command from the keyboard
int 
Driver::read( Command& command )
{
    // storage for loop variables
    char c;

    if ( !dataAvailable(kfd_) )
    {
        return TIMEOUT;
    }

    // get the next event from the keyboard
    // note the global scope of this library function
    // (as apposed the member function Driver::read() )
    if( ::read(kfd_, &c, 1) < 0 )
    {
//         network_->newRelativeCommand( 0.0, 0.0, 0.0 );
        command.resetAbsolute();
        return -1;
    }

    switch( c )
    {
        case KEYCODE_i:
//             network_->newCommandIncrement( +1, 0, 0 );
            command.resetIncremental();
            command.longitudinal = 1.0;
            break;
        case KEYCODE_k:
//             network_->newCommandIncrement( -1, 0, 0 );
            command.resetIncremental();
            command.longitudinal = -1.0;
            break;
        case KEYCODE_j:
//             network_->newCommandIncrement( 0, 0, +1 );
            command.resetIncremental();
            command.angular = 1.0;
            break;
        case KEYCODE_l:
//             network_->newCommandIncrement( 0, 0, -1 );
            command.resetIncremental();
            command.angular = -1.0;
            break;
        case KEYCODE_o:
//             network_->newRelativeCommand( 0.0, TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED );
            command.resetIncremental();
            command.isLongIncrement = false;
            command.longitudinal = 0.0;
            break;
        case KEYCODE_u:
//             network_->newRelativeCommand( TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED, 0.0 );
            command.resetIncremental();
            command.isAngularIncrement = false;
            command.angular = 0.0;
            break;
        case KEYCODE_1:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.1, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.1;
            }
            break;
        case KEYCODE_2:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.2, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.2;
            }
            break;
        case KEYCODE_3:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.3, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.3;
            }
            break;
        case KEYCODE_4:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.4, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.4;
            }
            break;
        case KEYCODE_5:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.5, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.5;
            }
            break;
        case KEYCODE_6:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.6, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.6;
            }
            break;
        case KEYCODE_7:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.7, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.7;
            }
            break;
        case KEYCODE_8:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.8, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.8;
            }
            break;
        case KEYCODE_9:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 0.9, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 0.9;
            }
            break;
        case KEYCODE_0:
            if ( enableStepInputs_ ) {
//                 network_->newRelativeCommand( 1.0, 0.0, 0.0 );
                command.resetIncremental();
                command.isLongIncrement = false;
                command.longitudinal = 1.0;
            }
            break;
        default:
            // any other key sends 'stop' command
//             network_->newRelativeCommand( 0.0, 0.0, 0.0 );
            command.resetAbsolute();
    }

    return 0;
}


} // namespace

extern "C" {
    hydrointerfaces::HumanInput2dFactory *createHumanInput2dDriverFactory()
    { return new humaninput2dkbdtermio::Factory; }
}
