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

#include <hydroiceutil/eventqueue.h>

using namespace std;

class TestEventQueueHolder : public hydroiceutil::EventQueueHolder
{
public: 
    TestEventQueueHolder() :
        events_(new hydroiceutil::EventQueue) {};

    // from EventQueueHolder
    virtual void postEvent( const hydroiceutil::EventPtr& e ) { events_->add( e ); };
private:
    hydroiceutil::EventQueuePtr events_;
};

int main(int argc, char * argv[])
{
    TestEventQueueHolder holder;

    cout<<"posting event ... ";
    holder.postEvent( new hydroiceutil::Event( 99 ) );
    cout<<"ok"<<endl;
    
    cout<<"posting event type ... ";
    holder.postEventWithType( 99 );
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
