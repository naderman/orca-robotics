/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_POWER_IMPL_H
#define ORCA_POWER_IMPL_H

#include <memory>
#include <orca/power.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>
#include <orcaice/topichandler.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Implements the orca::Power interface: Handles remote calls.
//!
class PowerImpl : public IceUtil::Shared
{
friend class PowerI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    PowerImpl( const std::string& interfaceTag, const orcaice::Context& context );
    //! constructor using interfaceName
    PowerImpl( const orcaice::Context& context, const std::string& interfaceName );
    ~PowerImpl();

    // local interface:
    //! May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PowerData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::PowerData& data );

private:
    void init();

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PowerData internalGetData() const;
    void internalSubscribe(const ::orca::PowerConsumerPrx&);
    void internalUnsubscribe(const ::orca::PowerConsumerPrx&);

    gbxiceutilacfr::Store<orca::PowerData> dataStore_;

    typedef orcaice::TopicHandler<orca::PowerConsumerPrx,orca::PowerData> PowerTopicHandler;
    std::auto_ptr<PowerTopicHandler> topicHandler_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;
};

typedef IceUtil::Handle<PowerImpl> PowerImplPtr;

}

#endif
