/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_KEYBOARD_NCURSES_DRIVER_H
#define ORCA2_TELEOP_KEYBOARD_NCURSES_DRIVER_H

#include <curses.h>

#include "../inputdriver.h"
#include "../displayhandler.h"

namespace teleop
{

class KeyboardNcurcesDriver : public InputDriver, public DisplayHandler
{
public:

    KeyboardNcurcesDriver( const InputDriver::Config &cfg );
    virtual ~KeyboardNcurcesDriver();

    virtual int enable();
    virtual int disable();

    // Blocks till new data is available
    virtual int read( orca::Velocity2dCommand& data );

    // from DisplayHandler
    virtual void displayEvent( const Event e );
    virtual void displayCommand( const orca::Velocity2dCommand& command,
                                 const bool vx=false, const bool vy=false, const bool w=false );

private:

    orca::Velocity2dCommand command_;

    Config config_;

    double deltaSpeed_;     // [m/s]
    double deltaTurnrate_;  // [rad/sec]

    void keyboardHelp();

    // obscure ncurses stuff
    WINDOW* mainwin_;

};

} // namespace

#endif
