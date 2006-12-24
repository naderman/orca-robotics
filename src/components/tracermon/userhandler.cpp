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

UserHandler::UserHandler( const orcaice::Context & context ) :
    events_(new orcaice::EventQueue),
    context_(context)
{
}

UserHandler::~UserHandler()
{
}

void 
UserHandler::enable( Network* network )
{
    assert( network || "pointer to network must be non-zero" );
    if ( !network ) {
        cout<<"null network pointer. exitting..."<<endl;
        exit(1);
    }
    network_ = network;
}

void 
UserHandler::newTraceMessage( const orca::TracerData & data )
{
    orcaice::EventPtr e = new NewTraceMessageEvent( data );
    events_->add( e );
}

// read commands from the keyboard. Launced in a separate thread.
void
UserHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    orcaice::EventPtr event;
    int timeoutMs = 500;
    
    //
    // Main loop
    //
    while ( isActive() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case NewTraceMessage : {
            //cout<<"focus changed event"<<endl;
            NewTraceMessageEventPtr e = NewTraceMessageEventPtr::dynamicCast( event );
            if ( !e ) {
                cout<<"failed to cast event to FocusChanged"<<endl;
                break;
            }
            cout<<e->data_.message<<endl;
        }        
        default : {
            cout<<"unknown display event "<<event->type()<<". Ignoring..."<<endl;
            break;
        }
        } // switch
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
