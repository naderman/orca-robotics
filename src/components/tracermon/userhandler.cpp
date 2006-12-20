/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "userhandler.h"

// "drivers"
#ifdef HAVE_KEYBOARD_TERMIO_DRIVER
    #include "kbd-termio/keyboardtermiodriver.h"
#endif
#ifdef HAVE_KEYBOARD_NCURSES_DRIVER
    #include "kbd-ncurses/keyboardncursesdriver.h"
#endif
#ifdef HAVE_JOYSTICK_DRIVER
    #include "joystick/joystickdriver.h"
#endif
// #include "fakedriver.h"

using namespace std;
using namespace orca;
using namespace tracermon;

UserHandler::UserHandler( const EventQueuePtr & myQueue, 
                          const EventQueuePtr & otherQueue, 
                          const orcaice::Context & context )
    : myQueue_(myQueue),
      otherQueue_(otherQueue),
      context_(context)
{
}

UserHandler::~UserHandler()
{
}

// read commands from the keyboard. Launced in a separate thread.
void
UserHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    EventPtr eventPtr = 0;
    int timeoutMs = 500;
    int ret = 0;
    
    //
    // Main loop
    //
//     cout<<"UserHandler: entering main loop"<<endl;
    while ( isActive() )
    {
        eventPtr = new Event( 0 );

        // block with timeout on user events (change in verbosity)
        ret = myQueue_->timedGet( eventPtr, timeoutMs );

        if ( ret ) {
            // process the event
//             cout<<"UserHandler: received event "<<eventPtr->type()<<endl;
        }
        else {
//             cout<<"UserHandler: timed out"<<endl;
        }
    } // end of main loop

    cout<<"UserHandler: exited main loop"<<endl;

    // reset the hardware
//     if ( driver_->disable() ) {
//         context_.tracer()->warning("failed to disable driver");
//     }

    } // try
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        context_.communicator()->destroy();
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "UserHandler: stopped.",5 );
}
