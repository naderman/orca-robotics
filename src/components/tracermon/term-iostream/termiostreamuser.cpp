/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "termiostreamuser.h"
// super-dodgy but can't make cmake recongnize included dir's properly
#include "../termutil/termutils.h"
#include "../events.h"

using namespace std;
using namespace orca;

namespace tracermon
{

TermIostreamUser::TermIostreamUser( const orcaice::Context & context ) :
    events_(new orcaice::EventQueue),
    context_(context)
{
}

TermIostreamUser::~TermIostreamUser()
{
}

void 
TermIostreamUser::enable( Network* network )
{
    assert( network || "pointer to network must be non-zero" );
    if ( !network ) {
        cout<<"null network pointer. exitting..."<<endl;
        exit(1);
    }
    network_ = network;
}

void 
TermIostreamUser::newTraceMessage( const orca::TracerData & data )
{
    orcaice::EventPtr e = new NewTraceMessageEvent( data );
    events_->add( e );
}

void 
TermIostreamUser::newVerbosityLevel( int error, int warn, int info, int debug ) 
{
}

void 
TermIostreamUser::newLocalTrace( const std::string& msg )
{
    cout<<"TRACE: "<<msg<<endl;
}

// read commands from the keyboard. Launced in a separate thread.
void
TermIostreamUser::run()
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
                cout<<"failed to cast event to NewTraceMessage"<<endl;
                break;
            }
            cout<<tracermon::toString(e->data_)<<endl;
            break;
        }        
        default : {
            cout<<"unknown display event "<<event->type()<<". Ignoring..."<<endl;
            break;
        }
        } // switch
    } // end of main loop

    cout<<"TermIostreamUser: exited main loop"<<endl;

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
    context_.tracer()->debug( "TermIostreamUser: stopped.",5 );
}

} // namespace
