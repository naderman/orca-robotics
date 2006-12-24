/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <Ice/Ice.h>

#include "eventqueue.h"

using namespace std;
using namespace orcaice;

void
EventQueue::add( const EventPtr & e )
{
    Lock sync(*this);
    events_.push_back( e );
//     cout<<"queue length : "<<events_.size()<<endl;

    notify();
}

void
EventQueue::get( EventPtr & e )
{
    Lock sync(*this);

    while( events_.empty() )
    {
        wait();
    }

    // got something
    e = events_.front();
    events_.pop_front();
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
        return true;
    }
    return false;
}

int
EventQueue::size() const
{
    Lock sync(*this);

    return events_.size();
}
