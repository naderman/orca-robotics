/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    PathFollower2dI( orcaice::PtrProxy<orca::PathFollower2dDataPtr> &pathPipe,
                     orcaice::Proxy<bool>                           &newPathArrivedPipe,
                     orcaice::Proxy<orca::Time>                     &activationPipe,
                     orcaice::Proxy<int>                            &wpIndexPipe,
                     const IceStorm::TopicPrx & topicPrx );

    // remote calls:

    // Get the current path
    virtual ::orca::PathFollower2dDataPtr getData(const ::Ice::Current& = ::Ice::Current()) const;

    // Set a path
    virtual void setData(const ::orca::PathFollower2dDataPtr &data, bool activateImmediately, const ::Ice::Current& = ::Ice::Current());

    virtual void activateNow(const ::Ice::Current& = ::Ice::Current());
    
    virtual int  getWaypointIndex(const ::Ice::Current& = ::Ice::Current()) const;

    // Subscribe people
    virtual void subscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());



private:

    // New paths from the outside world go in here
    orcaice::PtrProxy<orca::PathFollower2dDataPtr> &pathPipe_;

    // Let the component know that a new path has arrived
    orcaice::Proxy<bool>                           &newPathArrivedPipe_;

    // Time of Activation from the outside world goes in here
    orcaice::Proxy<orca::Time>                     &activationPipe_;

    // Progress info from the component goes in here
    orcaice::Proxy<int>                            &wpIndexPipe_;
    
    const IceStorm::TopicPrx topicPrx_;
};

}

#endif
