/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_INS_IMPL_H
#define ORCA_INS_IMPL_H

#include <orca/ins.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Implements the orca::Ins interface: Handles remote calls.
//!
class InsImpl : public IceUtil::Shared
{
friend class InsI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    InsImpl( const orca::InsDescription &descr,
                    const std::string              &interfaceTag, 
                    const orcaice::Context         &context );
    //! constructor using interfaceName
    InsImpl( const orca::InsDescription &descr,
                    const orcaice::Context         &context,
                    const std::string              &interfaceName );
    ~InsImpl();

    // local interface:
    //! May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::InsData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::InsData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::InsData internalGetData() const;
    ::orca::InsDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::InsConsumerPrx&);
    void internalUnsubscribe(const ::orca::InsConsumerPrx&);

    orca::InsDescription     descr_;
    gbxiceutilacfr::Store<orca::InsData> dataStore_;

    orca::InsConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string       interfaceName_;
    const std::string       topicName_;
    orcaice::Context        context_;
};
typedef IceUtil::Handle<InsImpl> InsImplPtr;

}

#endif
