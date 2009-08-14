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
#include <hydroiceutil/eventloop.h>

using namespace std;
using namespace hydroiceutil;

EventLoop::EventLoop( gbxutilacfr::Tracer& tracer, const JobQueue::Config& config ) :
    JobQueue( tracer, config )
{
}

void
EventLoop::postEvent( EventReceiver& receiver, const EventPtr &event, const std::string& signature, bool debug )
{
    JobPtr j = new EventJob( receiver, event, signature );
    if ( debug )
        j->isTracable_ = true;
    add( j );
}

void
EventLoop::postEvent( EventReceiver& receiver, int type, const std::string& signature, bool debug )
{
    JobPtr j = new EventJob( receiver, new hydroiceutil::Event(type), signature );
    if ( debug )
        j->isTracable_ = true;
    add( j );
}

void 
EventLoop::postEvent( const std::string& receiverId, const EventPtr &event, const std::string& senderId, bool debug )
{    
    IceUtil::Mutex::Lock lock(registryMutex_);

    std::map<std::string,EventReceiver*>::const_iterator it = registry_.find( receiverId );
    if ( it == registry_.end() )
        throw ReceiverIdNotExistException( ERROR_INFO, "Tried to post event to unregeistered receiver with ID="+receiverId );

    JobPtr j = new EventJob( *it->second, event, "to="+receiverId+" from="+senderId );
    if ( debug )
        j->isTracable_ = true;
    add( j );
}

void 
EventLoop::postEvent( const std::string& receiverId, int type, const std::string& senderId, bool debug )
{
    postEvent( receiverId, new hydroiceutil::Event(type), senderId, debug );
}

void 
EventLoop::postEventToAll( const EventPtr &event, const std::string& senderId, bool debug )
{
    IceUtil::Mutex::Lock lock(registryMutex_);

    std::map<std::string,EventReceiver*>::const_iterator it;
    for ( it=registry_.begin(); it!=registry_.end(); ++it ) {
        JobPtr j = new EventJob( *it->second, event, "to="+it->first+"(all) from="+senderId );
        if ( debug )
            j->isTracable_ = true;
        add( j );
    }
}

void 
EventLoop::postEventToAll( int type, const std::string& senderId, bool debug )
{
    postEventToAll( new hydroiceutil::Event(type), senderId, debug );
}

void 
EventLoop::addReceiver( const std::string& receiverId, EventReceiver* receiver )
{
    if ( receiver == 0 )
        throw gbxutilacfr::Exception( ERROR_INFO, "Tried to add null receiver with ID="+receiverId );

    IceUtil::Mutex::Lock lock(registryMutex_);
    // this inserts or overwrites
    registry_[receiverId] = receiver;
}

void 
EventLoop::removeReceiver( const std::string& receiverId )
{
    IceUtil::Mutex::Lock lock(registryMutex_);

    std::map<std::string,EventReceiver*>::iterator it = registry_.find( receiverId );
    if ( it != registry_.end() )
        registry_.erase( it );

    // if the id does not exist, ignore it.
}

EventReceiver* 
EventLoop::receiver( const std::string& receiverId )
{
    IceUtil::Mutex::Lock lock(registryMutex_);

    std::map<std::string,EventReceiver*>::const_iterator it = registry_.find( receiverId );
    if ( it == registry_.end() ) {
        return 0;
    }
    return it->second;
}
