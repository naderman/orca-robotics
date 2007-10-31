/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "keyboardiostreamdriver.h"
#include "network.h"

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

KeyboardIostreamDriver::KeyboardIostreamDriver( Network* network ) :
    network_(network)
{
}

KeyboardIostreamDriver::~KeyboardIostreamDriver()
{
}

int 
KeyboardIostreamDriver::enable()
{
    // display keymap
    keyboardHelp();

    //cout<<"enabled."<<endl;
    return 0;
}

int 
KeyboardIostreamDriver::disable()
{
    return 0;
}

// read next command from the keyboard
int 
KeyboardIostreamDriver::read()
{
    // storage for loop variables
    char c;

    // block until get a character + Enter
    cin >> c;

    switch( c )
    {
        case 'i':
            network_->newCommandIncrement( +1, 0, 0 );
            break;
        case 'k':
            network_->newCommandIncrement( -1, 0, 0 );
            break;
        case 'o':
            network_->newRelativeCommand( 0.0, TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED );
            break;
        case 'j':
            network_->newCommandIncrement( 0, 0, +1 );
            break;
        case 'l':
            network_->newCommandIncrement( 0, 0, -1 );
            break;
        case 'u':
            network_->newRelativeCommand( TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED, 0.0 );
            break;
        case 'q':
            //speed *= 1.1;
            //turn *= 1.1;
            break;
        case 'z':
            //speed *= 0.9;
            //turn *= 0.9;
            break;
        case 'w':
            //speed *= 1.1;
            break;
        case 'x':
            //speed *= 0.9;
            break;
        case 'e':
            //turn *= 1.1;
            break;
        case 'c':
            //turn *= 0.9;
            break;
        default:
            // any other key sends 'stop' command
            network_->newRelativeCommand( 0.0, 0.0, 0.0 );
    }

    return 0;
}
