/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_HOME_I_H
#define ORCAICE_HOME_I_H

#include <IceUtil/Mutex.h>

#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <orca/home.h>
#include "../home.h"
#include "../component.h" // for ComponentInterfaceFlag
#include "../context.h"

namespace orcaice
{

// An implementation of the (remote) Home interface.
class HomeI : public virtual orca::Home, public virtual orcaice::Home
{
public:
    HomeI( ComponentInterfaceFlag flag, const orcaice::Context& context );

    // orca::Home interface
    virtual orca::HomeData getInterfaces(const ::Ice::Current& ) const;
    virtual int getTimeUp(const ::Ice::Current& ) const;
    virtual orca::StringStringDict getProperties(const ::Ice::Current& ) const;

    // orcaice::Home interface
    virtual void addProvidedInterface( const orca::ProvidedInterface& iface );
    virtual void addRequiredInterface( const orca::RequiredInterface& iface );
    virtual void removeProvidedInterface( const std::string& name );
    virtual void removeRequiredInterface( const std::string& name );

private:
    // we update the timeUp field every time getInterfaces() is called.
    mutable orca::HomeData homeData_;
    
    orca::StringStringDict properties_;

    // start time is initialized in the constructor. the component's up time
    // is counted from here.
    gbxiceutilacfr::Timer upTimer_;

    // We may have multiple threads talking to us
    IceUtil::Mutex mutex_;
};

} // namespace

#endif
