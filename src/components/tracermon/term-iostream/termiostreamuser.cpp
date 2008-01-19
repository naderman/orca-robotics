/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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
    SubsystemThread( context.tracer(), context.status(), "UserThread" ), // alexm: is this the right name?
    events_(new hydroiceutil::EventQueue),
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
    hydroiceutil::EventPtr e = new NewTraceMessageEvent( data );
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
TermIostreamUser::walk()
{
    hydroiceutil::EventPtr event;
    int timeoutMs = 500;
    
    //
    // Main loop
    //
    while ( !isStopping() )
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
//         context_.tracer().warning("failed to disable driver");
//     }

    context_.tracer().debug( "TermIostreamUser: stopped.",5 );
}

} // namespace
