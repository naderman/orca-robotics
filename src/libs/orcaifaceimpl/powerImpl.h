/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POWER_IMPL_H
#define ORCA2_POWER_IMPL_H

#include <orca/power.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaiceutil/proxy.h>
#include <orcaice/context.h>

namespace orcaiceutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the orca::Power interface: Handles remote calls.
//!
class PowerImpl : public IceUtil::Shared
{
friend class PowerI;

public:
    //! constructor
    PowerImpl( const std::string& interfaceTag, 
            const orcaice::Context& context );
    ~PowerImpl();

    // local interface:
    //! May throw orcaiceutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( orcaiceutil::Thread* thread, int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PowerData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::PowerData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PowerData internalGetData() const;
    void internalSubscribe(const ::orca::PowerConsumerPrx&);
    void internalUnsubscribe(const ::orca::PowerConsumerPrx&);

    orcaiceutil::Proxy<orca::PowerData> dataProxy_;

    orca::PowerConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              tag_;
    orcaice::Context               context_;
};
typedef IceUtil::Handle<PowerImpl> PowerImplPtr;

}

#endif
