/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <stdio.h>
#include <sys/types.h>

#include "driver.h"

using namespace std;

namespace humaninput2dkbdiostream
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
    puts("anything else (except white space): stop");
    puts("!!! Press ENTER after each character to input command !!!");
    puts("-----------------------------------------");
};

Driver::Driver( const hydroutil::Context &context ) :
    context_(context)
{
}

Driver::~Driver()
{
}

int 
Driver::enable()
{
    // display keymap
    keyboardHelp();

    //cout<<"enabled."<<endl;
    return 0;
}

int 
Driver::disable()
{
    return 0;
}

// read next command from the keyboard
int 
Driver::read( Command& command )
{
    // storage for loop variables
    char c;

    const int stdin=0;
    if ( !dataAvailable(stdin) )
    {
        return TIMEOUT;
    }

    // block until get a character + Enter
    cin >> c;

    switch( c )
    {
        case 'i':
            command.resetIncremental();
            command.longitudinal = 1.0;
            break;
        case 'k':
            command.resetIncremental();
            command.longitudinal = -1.0;
            break;
        case 'j':
            command.resetIncremental();
            command.angular = 1.0;
            break;
        case 'l':
            command.resetIncremental();
            command.angular = -1.0;
            break;
        case 'o':
            command.resetIncremental();
            command.isLongIncrement = false;
            command.longitudinal = 0.0;
            break;
        case 'u':
            command.resetIncremental();
            command.isAngularIncrement = false;
            command.angular = 0.0;
            break;
        default:
            // any other key sends 'stop' command
            command.resetAbsolute();
    }

    return 0;
}

} // namespace

extern "C" {
    hydrointerfaces::HumanInput2dFactory *createHumanInput2dDriverFactory()
    { return new humaninput2dkbdiostream::Factory; }
}
