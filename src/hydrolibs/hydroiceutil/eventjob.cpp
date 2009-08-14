/*
 * Orca-Robotics Project: Components for robotics 
 *               http://!orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <hydroiceutil/eventjob.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;
using namespace hydroiceutil;

int EventJob::_warnDurationUs = -1;


EventJob::EventJob( EventReceiver& receiver, const EventPtr &e, const std::string& signature ) :
    receiver_(receiver),
    event_(e),
    signature_(signature)
{
}

EventJob::EventJob( EventReceiver& receiver, int type ) :
    receiver_(receiver),
    event_( new Event(type) )
{
}

void 
EventJob::execute()
{
    IceUtil::Time t;
    if ( _warnDurationUs >= 0 )
        t = IceUtil::Time::now();

    if ( !receiver_.event( event_ ) ) {
        stringstream ss;
        ss << "Event receiver did not accept event type "<<event_->type();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    if ( _warnDurationUs >= 0 && (IceUtil::Time::now()-t).toMicroSeconds()>_warnDurationUs )
        cout<<"************ EventJob "<<toString()<<" was processed "<<(IceUtil::Time::now()-t).toMicroSeconds()<<"us"<<endl;
}

std::string 
EventJob::toString() const 
{ 
    stringstream ss;
    ss << "EventJob: event type = " << event_->type() << ".";
    if ( !signature_.empty() )
        ss << " Signature: "<<signature_;
    return ss.str(); 
};
