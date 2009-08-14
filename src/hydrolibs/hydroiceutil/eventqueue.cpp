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

#include "eventqueue.h"

using namespace std;
using namespace hydroiceutil;


EventQueue::EventQueue( bool traceAddEvents, bool traceGetEvents ) :
    traceAddEvents_(traceAddEvents),
    traceGetEvents_(traceGetEvents)
{
}

void
EventQueue::setOptimizer( EventQueueOptimizerPtr optimizer )
{
    optimizer_ = optimizer;
}

void
EventQueue::add( const EventPtr & e )
{
    Lock sync(*this);
    events_.push_back( e );

    if ( traceAddEvents_ ) {
        cout<<"EventQueue::add() added event type="<<e->type()<<endl;
    }

    notify();
}

void
EventQueue::add( int type )
{
    add( new Event( type ) );
}

void
EventQueue::optimizedAdd( const EventPtr & e )
{
    Lock sync(*this);

    if ( !events_.size() || !optimizer_ || !optimizer_->combine( events_.back(), e ) ) 
    {   
        // comination did not happen for some reason, just add event to the queue
        events_.push_back( e );

        if ( traceAddEvents_ ) {
            cout<<"EventQueue::addOptimized() added event type="<<e->type()<<" without optimization"<<endl;
        }
        notify();
    }
}

void
EventQueue::get( EventPtr & e )
{
    Lock sync(*this);

    while( events_.empty() )
    {
        wait();
    }

    e = events_.front();
    events_.pop_front();

    if ( traceGetEvents_ ) {
        cout<<"EventQueue::get() got event type="<<e->type()<<endl;
    }
}

bool
EventQueue::timedGet( EventPtr & e, int timeoutMs )
{
    IceUtil::Time wakeupTime = IceUtil::Time::now() + 
                               IceUtil::Time::milliSeconds( timeoutMs );

    Lock sync(*this);

    while( events_.empty() && IceUtil::Time::now()<wakeupTime )
    {
        timedWait( wakeupTime - IceUtil::Time::now() );
    }

    if ( !events_.empty() ) {
        // got something
        e = events_.front();
        events_.pop_front();

        if ( traceGetEvents_ ) {
            cout<<"EventQueue::get() got event type="<<e->type()<<endl;
        }
        return true;
    }
    return false;
}

void
EventQueue::clear()
{
    Lock sync(*this);

    return events_.clear();
}

int
EventQueue::size() const
{
    Lock sync(*this);

    return events_.size();
}
