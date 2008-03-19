/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef TERM_NCURSES_USER_DRIVER_H
#define TERM_NCURSES_USER_DRIVER_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>
#include <hydroiceutil/eventqueue.h>
#include "../user.h"
#include "../network.h"

#include <curses.h>
#include <form.h>

namespace tracermon
{

class TermNcursesUser : public gbxsickacfr::gbxiceutilacfr::SafeThread, public tracermon::User
{
public:

    TermNcursesUser( const orcaice::Context & context );
    virtual ~TermNcursesUser();

    // from Thread
    virtual void walk();

    // from User
    virtual void enable( Network* network );

    virtual void newTraceMessage( const orca::TracerData & data );
    virtual void newLocalTrace( const std::string& msg );

    virtual void previousField();
    virtual void nextField();
    virtual void valueUp();
    virtual void valueDown();
    virtual void action();

    virtual void newVerbosityLevel( int error, int warn, int info, int debug );
    
private:

    Network* network_;
    hydroiceutil::EventQueuePtr events_;
   
    orcaice::Context context_;

    gbxsickacfr::gbxiceutilacfr::SafeThread* inputMainThread_;

    // obscure ncurses stuff
    WINDOW* mainwin_;
    WINDOW* statuswin_;
    WINDOW* menuwin_;
    FIELD*  menufields_[16];
    FORM*   menuform_;

    // gui state
    int menuPos_;
    int menuCurrState_[4];
    int menuNewState_[4];

    // utilities
    void displayTrace( const orca::TracerData & data );
    void displayLocalTrace( const std::string& msg );
};

} // namespace

#endif
