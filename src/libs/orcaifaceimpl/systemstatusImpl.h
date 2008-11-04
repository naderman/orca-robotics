/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_SYSTEMLEVELSTATUS_IMPL_H
#define ORCA_SYSTEMLEVELSTATUS_IMPL_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/systemstatus.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Sets up an implementation of the remote interface, and handles remote calls.
//!
class SystemStatusImpl : public IceUtil::Shared
{
friend class SystemStatusI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    SystemStatusImpl( const std::string                     &interfaceTag,
                           const orcaice::Context                &context );
    //! constructor using interfaceName
    SystemStatusImpl( const orcaice::Context                &context,
                           const std::string                     &interfaceName );                        
    ~SystemStatusImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::SystemStatusData &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::SystemStatusData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::SystemStatusData     internalGetData() const;
    void internalSubscribe(const ::orca::SystemStatusConsumerPrx& );
    void internalUnsubscribe(const ::orca::SystemStatusConsumerPrx& );

    // Holds the latest data
    gbxiceutilacfr::Store<orca::SystemStatusData> dataStore_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::SystemStatusConsumerPrx publisherPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string  interfaceName_;
    const std::string  topicName_;
    orcaice::Context   context_;
};
typedef IceUtil::Handle<SystemStatusImpl> SystemStatusImplPtr;

} // namespace

#endif
