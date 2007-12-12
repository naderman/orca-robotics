/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include <curses.h>
#include "inputthread.h"

using namespace std;
using namespace tracermon;

MainThread::MainThread( Network& network, User& user, const orcaice::Context& context ) :
    SafeThread( context.tracer(), context.status(), "InputThread" ),
    network_(network),
    user_(user)
{
}

MainThread::~MainThread()
{
}

// read commands from the keyboard. Launced in a separate thread.
void
MainThread::walk()
{    
    // storage for loop variables
    int ch;
//     char keych[2] = {0};

    //
    // Main loop
    //
    while ( !isStopping() )
    {    
        //
        // get the next event from the keyboard
        //
        ch = getch();
        
        // If a printable character
        switch ( ch )
        {
        case  KEY_LEFT :
            user_.previousField();
            break;
        case  KEY_RIGHT :
            user_.nextField();
            break;
        case  KEY_UP :
            user_.valueUp();
            break;
        case  KEY_DOWN :
            user_.valueDown();
            break;
	case '\n':
        case  '\r' :
        case  KEY_ENTER :
//             cout<<"detected action"<<endl;
            user_.action();
            break;
        } // switch
        
//         IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
//         cout<<"detected action"<<endl;
//         user_.action();

    } // end of main loop

    cout<<"MainThread: exited main loop"<<endl;

    // reset the hardware
//     if ( driver_->disable() ) {
//         context_.tracer().warning("failed to disable driver");
//     }
}
