/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_HOME_IMPL_H
#define ORCAICE_HOME_IMPL_H

#include <orcaice/home.h>

#include <orca/home.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <orcaice/context.h>

// class gbxiceutilacfr::Thread;

namespace orcaice
{

class HomeImpl : public virtual orcaice::Home, public IceUtil::Shared
{
friend class HomeI;

public:
    HomeImpl( const orcaice::Context& context );
    ~HomeImpl();

    // orcaice::Home interface
    virtual void addProvidedInterface( const orca::ProvidedInterface& iface );
    virtual void addRequiredInterface( const orca::RequiredInterface& iface );
    virtual void removeProvidedInterface( const std::string& name );
    virtual void removeRequiredInterface( const std::string& name );

    void initInterface( const orcaice::Context& context );
//     void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

private:

    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::HomeData internalGetInterfaces();
    int internalGetTimeUp();
//     orca::StringStringDict internalGetProperties();

    gbxiceutilacfr::Store<orca::HomeData> dataStore_;
    // we don't need to protect it if we only read from it.
    // todo: make a special read-only class.
    // start time is initialized in the constructor. the component's up time
    // is counted from here.
    gbxiceutilacfr::Timer upTimer_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              interfaceName_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<HomeImpl> HomeImplPtr;

} // namespace

#endif
