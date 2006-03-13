/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include <stdlib.h>
#include <ctype.h>
#include <curses.h>

#include "keyboardncursesdriver.h"
#include "asciichar.h"


using namespace std;
using namespace orca;
using namespace teleop;


KeyboardNcurcesDriver::KeyboardNcurcesDriver( const InputDriver::Config &cfg ) :
        config_(cfg)
{
    // init internal data storage
    command_ = new Velocity2dCommand;
    command_->motion.v.x = 0.0;
    command_->motion.v.y = 0.0;
    command_->motion.w = 0.0;

    // one key stroke changes commands by these values
    deltaSpeed_ = 0.05;     // [m/s]
    deltaTurnrate_ = DEG2RAD(2.0);  // [rad/sec]
}

KeyboardNcurcesDriver::~KeyboardNcurcesDriver()
{
}

int KeyboardNcurcesDriver::enable()
{
   // int ch;

    //  Initialize ncurses
    if ( (mainwin_ = initscr()) == NULL ) {
        cerr<<"Error initializing ncurses."<<endl;
        return -1;
    }

    // Initialize colours
    start_color();
    
    // Make sure we are able to do what we want. If
    // has_colors() returns FALSE, we cannot use colours.
    // COLOR_PAIRS is the maximum number of colour pairs
    // we can use. We use 13 in this program, so we check
    // to make sure we have enough available.

    if ( !has_colors() && COLOR_PAIRS < 13 ) {
        // shouldn't happen
        return -1;
    }

    // Initialize a bunch of colour pairs, where:
    //     init_pair(pair number, foreground, background);
    // specifies the pair.

    init_pair(1,  COLOR_BLACK,   COLOR_WHITE);
    init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
    
    color_set( 2, NULL );
    mvaddstr( 1, 4, "Orca2 Teleop Component");

    color_set( 1, NULL );
    
    // Turn off key echoing
    noecho();
    // Enable the keypad for non-char keys
    keypad( mainwin_, true );

    //  Print a prompt and refresh() the screen
    mvaddstr(5, 10, "Press a key ('q' to quit)...");
    refresh();
    
    // display keymap
    //keyboardHelp();
    
    //cout<<"enabled."<<endl;
    return 0;
}

int KeyboardNcurcesDriver::disable()
{
    //  Clean up after ourselves
    delwin(mainwin_);
    endwin();
    refresh();
    return 0;
}

// read next command from the keyboard
int KeyboardNcurcesDriver::read( orca::Velocity2dCommandPtr &data )
{
    // storage for loop variables
    int ch;
    char keych[2] = {0};

    //
    // get the next event from the keyboard
    //
    ch = getch();
    
    // If a printable character
    if ( isprint(ch) && !(ch & KEY_CODE_YES)) {
        keych[0] = ch;
    }
    mvprintw( 7, 10, "You pressed: 0x%x (%s)", ch, keych );
    refresh();
    
/*
    char c = KEYCODE_i;
    
    // get the next event from the keyboard
    if( read(kfd_, &c, 1) < 0 )
    {
        command_->motion.v.x = 0.0;
        command_->motion.v.y = 0.0;
        command_->motion.w = 0.0;

        string errString = "failed to read from keyboard";
        throw orcaice::HardwareException( ERROR_INFO, errString );
    }
*/
/*
    switch( c )
    {
        case KEYCODE_i:
            command_->motion.v.x += deltaSpeed_;
            break;
        case KEYCODE_k:
            command_->motion.v.x -= deltaSpeed_;
            break;
        case KEYCODE_o:
            command_->motion.v.x = 0.0;
            break;
        case KEYCODE_j:
            command_->motion.w += deltaTurnrate_;
            break;
        case KEYCODE_l:
            command_->motion.w -= deltaTurnrate_;
            break;
        case KEYCODE_u:
            command_->motion.w = 0.0;
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
            command_->motion.v.x = 0.0;
            command_->motion.v.y = 0.0;
            command_->motion.w = 0.0;
    }
*/
    // apply max limits
    if ( fabs(command_->motion.v.x) > config_.maxSpeed ) {
        command_->motion.v.x =
                (command_->motion.v.x / fabs(command_->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(command_->motion.w) > config_.maxTurnrate ) {
        command_->motion.w =
                (command_->motion.w / fabs(command_->motion.w)) * config_.maxTurnrate;
    }

    // return updated command
    // ice_clone() doesn't work for some reason
    //data = command_->ice_clone();
    //data->timeStamp = ?;
    data->motion.v.x = command_->motion.v.x;
    data->motion.v.y = command_->motion.v.y;
    data->motion.w = command_->motion.w;

    return 0;
}

void KeyboardNcurcesDriver::displayEvent( const Event e )
{
    switch ( e )
    {
    case SentNewCommand :
    {
        cout<<"C"<<flush;
        break;
    }
    case SentRepeatCommand :
    {
        cout<<"."<<flush;
        break;
    }
    default :
    {
        cout<<"x"<<flush;
    }
    }
}

void KeyboardNcurcesDriver::displayCommand( const orca::Velocity2dCommandPtr & command,
                                 const bool vx, const bool vy, const bool w )
{
    cout<<"C"<<flush;
    
//     cout<<"\n"<<command;
//     
//     if ( vx ) cout<<"Vx LIMITED. ";
//     if ( vy ) cout<<"Vy LIMITED. ";
//     if ( w ) cout<<"W LIMITED. ";
// 
//     cout<<endl;
}

void KeyboardNcurcesDriver::keyboardHelp()
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
