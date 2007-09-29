/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RANGESCANNER2D_IMPL_H
#define ORCA2_RANGESCANNER2D_IMPL_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/rangescanner2d.h>

// utilities
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>

namespace orcaiceutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Sets up an implementation of the remote interface, and handles remote calls.
//!
class RangeScanner2dImpl : public IceUtil::Shared
{
friend class RangeScanner2dI;

public:
    //! constructor
    RangeScanner2dImpl( const orca::RangeScanner2dDescription &descr,
                         const std::string                     &interfaceTag,
                         const orcaice::Context                &context );
    ~RangeScanner2dImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw orcaiceutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( orcaiceutil::Thread* thread, int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::RangeScanner2dDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::RangeScanner2dDataPtr &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::RangeScanner2dDataPtr     internalGetData() const;
    ::orca::RangeScanner2dDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::RangeScanner2dConsumerPrx& );
    void internalUnsubscribe(const ::orca::RangeScanner2dConsumerPrx& );

    // Holds the latest data
    orcaice::PtrProxy<orca::RangeScanner2dDataPtr> dataProxy_;

    orca::RangeScanner2dDescription descr_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::RangeScanner2dConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    std::string        interfaceTag_;
    orcaice::Context   context_;
};
typedef IceUtil::Handle<RangeScanner2dImpl> RangeScanner2dImplPtr;

} // namespace

#endif
