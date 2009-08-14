/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/Time.h>
#include <hydroiceutil/eventscheduler.h>
#include <hydroiceutil/simpletracer.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

class TestEventReceiver : public hydroiceutil::EventReceiver
{
public:        
    virtual bool event( const hydroiceutil::EventPtr& e )
    {
        // wait until the "final" event
        if ( e->type() == 99 )
            store_.set( true );
        return true;
    };
    
    gbxiceutilacfr::Store<bool> store_;
};

class RecursiveEventReceiver : public hydroiceutil::EventReceiver
{
public:     
    RecursiveEventReceiver( const hydroiceutil::EventSchedulerPtr& es ) :
        es_(es),
        done_(false) {};
    virtual bool event( const hydroiceutil::EventPtr& e )
    {
        if ( !done_ ) {
            es_->scheduleEvent( *(hydroiceutil::EventReceiver*)this, 99, 0.005 );
            done_ = true;
        }
        return true;
    };
    hydroiceutil::EventSchedulerPtr es_;
    bool done_;
    gbxiceutilacfr::Store<bool> store_;
};

int main(int argc, char * argv[])
{
    hydroiceutil::SimpleTracer tracer;
 
//    cout<<"starting job queue with illegal parameters ... ";
//     {
//         hydroiceutil::JobQueue::Config config;
//         config.threadPoolSize = -1;
//         config.queueSizeWarn = 1;
//         config.traceAddEvents = true;
//         config.traceDoneEvents = true;
// 
//         try {
//             hydroiceutil::JobQueue jobQueue( tracer, config );
//             cout<<" failed. Should've thrown an exception."<<endl;
//             return EXIT_FAILURE;
//         }
//         catch ( const gbxutilacfr::Exception& e ) {
//         }
//     }
//     cout<<"ok"<<endl;

    cout<<"starting event scheduler ... ";
    hydroiceutil::EventScheduler::Config config;
    config.pollIntervalMs = 200;
    config.traceScheduledEvents = true;
    config.traceDispatchedEvents = true;
    hydroiceutil::EventSchedulerPtr eventScheduler = new hydroiceutil::EventScheduler( tracer, config );
    cout<<"ok"<<endl;

    cout<<"scheduling a single quick event ... "<<endl;
    {
        TestEventReceiver receiver;
        eventScheduler->scheduleEvent( receiver, 99, 0.05 );
        bool isDone = false;
        if ( receiver.store_.getNext( isDone, 5000 ) ) {
            cout<<"getNext timed out"<<endl;
            return EXIT_FAILURE;
        }
        if ( !isDone ) {
            cout<<"the correct value was not set by the event"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"scheduling several delayed events ... "<<endl;
    {
        TestEventReceiver receiver;
        eventScheduler->scheduleEvent( receiver, 11, 0.3 );
        eventScheduler->scheduleEvent( receiver, 22, 0.3 );
        eventScheduler->scheduleEvent( receiver, 99, 0.33 );

        bool isDone = false;
        if ( receiver.store_.getNext( isDone, 5000 ) ) {
            cout<<"getNext timed out"<<endl;
            return EXIT_FAILURE;
        }
        if ( !isDone ) {
            cout<<"the correct value was not set by the event"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"scheduling events recursively ... "<<endl;
    {
        RecursiveEventReceiver receiver( eventScheduler );
        eventScheduler->scheduleEvent( receiver, 99, 0.005 );
    }
    cout<<"ok"<<endl;

    cout<<"stopping scheduler ... ";
    gbxiceutilacfr::stopAndJoin( eventScheduler );
    cout<<"ok"<<endl;
    return EXIT_SUCCESS;
}
