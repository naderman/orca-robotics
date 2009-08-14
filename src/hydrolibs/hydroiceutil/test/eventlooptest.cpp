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
#include <hydroiceutil/eventloop.h>
#include <hydroiceutil/simpletracer.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

class TestReceiver : public hydroiceutil::EventReceiver
{
public:
    virtual bool event( const hydroiceutil::EventPtr& e )
    {
        cout<<"******** TestReceiver received event type="<<e->type()<<endl;
        return true;
    };

};

int main(int argc, char * argv[])
{
    cout<<"starting event loop ... ";
    hydroiceutil::SimpleTracer tracer;
    hydroiceutil::JobQueue::Config config;
    config.threadPoolSize = 1;
    config.queueSizeWarn = 1;
    config.traceAddEvents = true;
    config.traceDoneEvents = true;
    hydroiceutil::EventLoop eventLoop( tracer, config );
    cout<<"ok"<<endl;

    TestReceiver r0, r1;

    cout<<"posting event to reciever ... "<<endl;
    int eventId = 11;
    cout<<"1 receiver should get event "<<eventId<<endl;
    eventLoop.postEvent( r0, eventId ); 

        // sleep for a bit, to see the printout of the job
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
    cout<<"ok"<<endl;

    cout<<"adding a receiver with new ID ... ";
    eventLoop.addReceiver( "a", &r0 );
    if ( &r0 != eventLoop.receiver( "a" ) ) {
        cout<<" failed. Could not retrieve receiver after storing"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
   
    cout<<"posting event to reciever by ID ... "<<endl;
    eventId = 22;
    cout<<"1 receiver should get event "<<eventId<<endl;
    eventLoop.postEvent( "a", eventId );

        // sleep for a bit, to see the printout of the job
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100)); 
    cout<<"ok"<<endl;

    cout<<"adding another receiver ... ";
    eventLoop.addReceiver( "b", &r1 );
    if ( &r1 != eventLoop.receiver( "b" ) ) {
        cout<<" failed. Could not retrieve receiver after storing"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"posting event to all registered recievers ... "<<endl;
    eventId = 33;
    cout<<"2 receiver should get event "<<eventId<<endl;
    eventLoop.postEventToAll( eventId ); 

        // sleep for a bit, to see the printout of the job
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
    cout<<"ok"<<endl;

    cout<<"adding same receiver with different ID (aliasing)  ... ";
    eventLoop.addReceiver( "B", &r1 );
    if ( &r1 != eventLoop.receiver( "B" ) ) {
        cout<<" failed. Could not retrieve receiver after storing"<<endl;
        return EXIT_FAILURE;
    }
    if ( &r1 != eventLoop.receiver( "b" ) ) {
        cout<<" failed. Could not retrieve the original receiver after aliasing."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"posting event to the alias ... "<<endl;
    eventId = 44;
    cout<<"1 receiver should get event "<<eventId<<endl;
    eventLoop.postEvent( "B", eventId );

        // sleep for a bit, to see the printout of the job
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100)); 
    cout<<"ok"<<endl;

    cout<<"posting event to all registered recievers ... "<<endl;
    eventId = 55;
    cout<<"3 receiver should get event "<<eventId<<endl;
    eventLoop.postEventToAll( eventId ); 

        // sleep for a bit, to see the printout of the job
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
    cout<<"ok"<<endl;

    cout<<"removing an existing receiver ... ";
    eventLoop.removeReceiver( "a" );
    if ( 0 != eventLoop.receiver( "a" ) ) {
        cout<<" failed. Could not remove receiver"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    cout<<"adding receiver with existing ID (replacement) ... ";
    eventLoop.addReceiver( "a", &r1 );
    if ( &r1 != eventLoop.receiver( "a" ) ) {
        cout<<" failed. Could not retrieve receiver after re-pointing"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"adding null receiver ... ";
    try {
        eventLoop.addReceiver( "a", 0 );
        cout<<" failed. Should not be able to store null receiver."<<endl;
        return EXIT_FAILURE;
    }
    catch ( gbxutilacfr::Exception& ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"removing non-existing receiver ... ";
    eventLoop.removeReceiver( "a" );
    if ( 0 != eventLoop.receiver( "a" ) ) {
        cout<<" failed. Wrong result after trying to remove a non-existing receiver"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
