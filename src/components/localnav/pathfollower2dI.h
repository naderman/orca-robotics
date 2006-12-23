/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PATH_FOLLOWER_2D_I_H
#define ORCA2_PATH_FOLLOWER_2D_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>
#include <orcaice/ptrproxy.h>

// include provided interfaces
#include <orca/pathfollower2d.h>

#include "pathmaintainer.h"

namespace localnav {

//
// Implements the PathFollower interface: Handles all our remote calls.
//
class PathFollower2dI : public orca::PathFollower2d
{
public:
    PathFollower2dI( orcaice::Proxy<orca::PathFollower2dData> &pathPipe,
                     orcaice::Proxy<bool>                           &newPathArrivedPipe,
                     orcaice::Proxy<orca::Time>                     &activationPipe,
                     orcaice::Proxy<int>                            &wpIndexPipe,
                     orcaice::Proxy<bool>                           &enabledPipe,
                     const IceStorm::TopicPrx & topicPrx );

    // remote calls:

    // Get the current path
    virtual ::orca::PathFollower2dData getData(const ::Ice::Current& = ::Ice::Current()) const;

    // Set a path
    virtual void setData(const ::orca::PathFollower2dData& data, bool activateImmediately, const ::Ice::Current& = ::Ice::Current());

    virtual void activateNow(const ::Ice::Current& = ::Ice::Current());
    
    virtual int  getWaypointIndex(const ::Ice::Current& = ::Ice::Current()) const;

    virtual void setEnabled( bool enabled, const ::Ice::Current& = ::Ice::Current() );
    virtual bool enabled(const ::Ice::Current& = ::Ice::Current()) const;

    // Subscribe people
    virtual void subscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());



private:

    // New paths from the outside world go in here
    orcaice::Proxy<orca::PathFollower2dData> &pathPipe_;

    // Let the component know that a new path has arrived
    orcaice::Proxy<bool>                           &newPathArrivedPipe_;

    // Time of Activation from the outside world goes in here
    orcaice::Proxy<orca::Time>                     &activationPipe_;

    // Progress info from the component goes in here
    orcaice::Proxy<int>                            &wpIndexPipe_;

    // Allow external en/dis-able
    orcaice::Proxy<bool>                           &enabledPipe_;
    
    const IceStorm::TopicPrx topicPrx_;
};

}

#endif
