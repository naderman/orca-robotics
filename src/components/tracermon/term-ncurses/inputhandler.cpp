/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include <curses.h>
#include "inputhandler.h"

using namespace std;
using namespace tracermon;

InputHandler::InputHandler( Network& network, User& user ) :
    network_(network),
    user_(user)
{
}

InputHandler::~InputHandler()
{
}

// read commands from the keyboard. Launced in a separate thread.
void
InputHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // storage for loop variables
    int ch;
//     char keych[2] = {0};

    //
    // Main loop
    //
    while ( isActive() )
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

    cout<<"InputHandler: exited main loop"<<endl;

    // reset the hardware
//     if ( driver_->disable() ) {
//         context_.tracer()->warning("failed to disable driver");
//     }

    } // try
    catch ( ... )
    {
        cerr<<"unexpected exception from somewhere."<<endl;
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
