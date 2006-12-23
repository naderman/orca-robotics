/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_HOME_I_H
#define ORCAICE_HOME_I_H

#include <Ice/Ice.h>
#include <IceUtil/Time.h>

// include provided interfaces
#include <orca/home.h>

namespace orcaice
{

// An implementation of the (remote) Home interface.
// Contains no local public API.
class HomeI : public virtual orca::Home
{
public:
    HomeI( const orca::ComponentData & compData, const Ice::PropertyDict & props );

    virtual orca::HomeData getInterfaces(const ::Ice::Current& ) const;

    virtual int getTimeUp(const ::Ice::Current& ) const;
    
    virtual orca::ComponentProperties getProperties(const ::Ice::Current& ) const;

private:
    // we update the timeUp field every time getInterfaces() is called.
    mutable orca::HomeData homeData_;
    
    orca::ComponentProperties properties_;

    // start time is initialized in the constructor. the component's up time
    // is counted from here.
    IceUtil::Time startTime_;
};

} // namespace

#endif
