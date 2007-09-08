/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CAMERA_IMPL_H
#define ORCA2_CAMERA_IMPL_H

#include <orca/camera.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaice {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the orca::Camera interface. Handles remote calls.
//!
class CameraImpl : public IceUtil::Shared
{
friend class CameraI;

public:
    //! constructor
    CameraImpl( const orca::CameraDescription& descr,
                 const std::string& interfaceTag, 
                 const orcaice::Context& context );
    ~CameraImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw orcaice::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( orcaice::Thread* thread, int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::CameraData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::CameraData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::CameraData internalGetData() const;
    ::orca::CameraDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::CameraConsumerPrx&);
    void internalUnsubscribe(const ::orca::CameraConsumerPrx&);

    orca::CameraDescription     descr_;
    orcaice::Proxy<orca::CameraData> dataProxy_;

    orca::CameraConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              interfaceTag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<CameraImpl> CameraImplPtr;

}

#endif
