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

namespace goalplanner {

//
// Implements the PathFollower interface: Handles all our remote calls.
//
class PathFollower2dI : public orca::PathFollower2d
{
public:
    PathFollower2dI( orcaice::PtrProxy<orca::PathFollower2dDataPtr> &pathPipe,
                     orcaice::Proxy<bool> &activationPipe,
                     orcaice::PtrProxy<orca::Localise2dDataPtr> &localiseDataBuffer,
                     orca::PathFollower2dPrx localNavPrx,
                     const IceStorm::TopicPrx & topicPrx );

    // remote calls:

    // Get the current path
    virtual ::orca::PathFollower2dDataPtr getData(const ::Ice::Current& = ::Ice::Current()) const;

    // Set a path
    virtual void setData(const ::orca::PathFollower2dDataPtr &data, bool activateImmediately, const ::Ice::Current& = ::Ice::Current());

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
    orcaice::PtrProxy<orca::PathFollower2dDataPtr> &pathPipe_;
    
    // Are we activated?
    orcaice::Proxy<bool> &activationPipe_;
    
    // Localise data buffer required to throw an exception if we're not localized and someone gives us a path from the outside
    orcaice::PtrProxy<orca::Localise2dDataPtr> &localiseDataBuffer_;
    
    orca::PathFollower2dPrx localNavPrx_;
    
    const IceStorm::TopicPrx topicPrx_;
};

}

#endif
