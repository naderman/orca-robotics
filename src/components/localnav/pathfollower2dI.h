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
    PathFollower2dI( const std::string              &ifaceTag,
                     const orcaice::Context         &context );

    // remote calls:

    // Get the current path
    virtual ::orca::PathFollower2dData getData(const ::Ice::Current& = ::Ice::Current()) const;

    // Set a path
    virtual void setData(const ::orca::PathFollower2dData& data,
                         bool activateImmediately,
                         const ::Ice::Current& = ::Ice::Current());

    virtual void activateNow(const ::Ice::Current& = ::Ice::Current());
    
    virtual int  getWaypointIndex(const ::Ice::Current& = ::Ice::Current()) const;
    
    virtual ::orca::Time getAbsoluteStartTime(const Ice::Current&) const;
    
    virtual double getRelativeStartTime(const Ice::Current&) const;

    virtual void setEnabled( bool enabled, const ::Ice::Current& = ::Ice::Current() );
    virtual bool enabled(const ::Ice::Current& = ::Ice::Current()) const;

    // Subscribe people
    virtual void subscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    //
    // Local calls
    //
    void initInterface();

    void localSetWaypointIndex( int index );
    void localSetData( const orca::PathFollower2dData &path );
    bool localIsEnabled() const;
    
    // local access to proxies
    orcaice::Proxy<orca::PathFollower2dData> &pathProxy()           { return pathProxy_; }
    orcaice::Proxy<bool>                     &newPathArrivedProxy() { return newPathArrivedProxy_; }
    orcaice::Proxy<orca::Time>               &activationProxy()     { return activationProxy_; }

private:

    // New paths from the outside world go in here
    orcaice::Proxy<orca::PathFollower2dData>        pathProxy_;

    // Let the component know that a new path has arrived
    orcaice::Proxy<bool>                            newPathArrivedProxy_;

    // Time of Activation from the outside world goes in here
    orcaice::Proxy<orca::Time>                      activationProxy_;

    // Progress info from the component goes in here
    orcaice::Proxy<int>                             wpIndexProxy_;

    // Allow external en/dis-able
    orcaice::Proxy<bool>                            enabledProxy_;
    
    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::PathFollower2dConsumerPrx  consumerPrx_;

    std::string                    ifaceTag_;
    orcaice::Context               context_;
};

}

#endif
