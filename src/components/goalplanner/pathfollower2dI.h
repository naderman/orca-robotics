/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PATH_FOLLOWER_2D_I_H
#define ORCA_PATH_FOLLOWER_2D_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>

// include provided interfaces
#include <orca/pathfollower2d.h>

namespace goalplanner {

//
// Implements the PathFollower interface: Handles all our remote calls.
//
class PathFollower2dI : public orca::PathFollower2d
{
public:
    PathFollower2dI( gbxiceutilacfr::Store<orca::PathFollower2dData> &pathPipe,
                     gbxiceutilacfr::Store<bool> &activationPipe,
                     orca::PathFollower2dPrx localNavPrx );

    // remote calls:

    // Get the current path
    virtual ::orca::PathFollower2dData getData(const ::Ice::Current& = ::Ice::Current()) const;

    // Set a path
    virtual void setData(const ::orca::PathFollower2dData& data, bool activateImmediately, const ::Ice::Current& = ::Ice::Current());

    virtual void activateNow(const ::Ice::Current& = ::Ice::Current());
    
    virtual int  getWaypointIndex(const ::Ice::Current& = ::Ice::Current()) const;
    
    virtual bool getAbsoluteActivationTime(orca::Time &activationTime, const Ice::Current&) const;
    
    virtual bool getRelativeActivationTime(double &secondsSinceActivation, const Ice::Current&) const;
            

    virtual void setEnabled( bool enabled, const ::Ice::Current& = ::Ice::Current() );
    virtual bool enabled(const ::Ice::Current& = ::Ice::Current()) const;

    // Subscribe people
    virtual void subscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::PathFollower2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());



private:

    // New paths from the outside world go in here
    gbxiceutilacfr::Store<orca::PathFollower2dData> &pathPipe_;
    
    // Are we activated?
    gbxiceutilacfr::Store<bool> &activationPipe_;
    
    // because goalplanner is acting like a transparent proxy to localnav, we need to pass on requests to localnav and thus need a remote object
    orca::PathFollower2dPrx localNavPrx_;
};

}

#endif
