/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_CAMERA_CAMERA_IMPL_H
#define ORCA_CAMERA_CAMERA_IMPL_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/camera.h>

// utilities
#include <orcaice/ptrstore.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Sets up an implementation of the remote interface, and handles remote calls.
//!
class CameraImpl : public IceUtil::Shared
{
friend class CameraI;

public:

    //! constructor using interfaceTag (may throw ConfigFileException)
    CameraImpl( const orca::CameraDescriptionPtr &descr,
                        const std::string                     &interfaceTag,
                        const orcaice::Context                &context );
    //! constructor using interfaceName
    CameraImpl( const orca::CameraDescriptionPtr &descr,
                        const orcaice::Context                &context,
                        const std::string                     &interfaceName );
    ~CameraImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. Before every attempt checks if the thread was stopped. After every
    //! If a non-empty subsystem name is supplied, sends a Status heartbeat after each attempt 
    //! (@see gbxutilacfr::Status).
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::CameraDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::CameraDataPtr &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::ImageDataPtr     internalGetData() const;
    ::orca::ImageDescriptionPtr internalGetDescription() const;
    void internalSubscribe(const ::orca::ImageConsumerPrx& );
    void internalUnsubscribe(const ::orca::ImageConsumerPrx& );

    // Holds the latest data
    orcaice::PtrStore<orca::CameraDataPtr> dataStore_;

    orca::CameraDescriptionPtr descr_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::ImageConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        interfaceName_;
    const std::string        topicName_;
    orcaice::Context   context_;
};
typedef IceUtil::Handle<CameraImpl> CameraImplPtr;

} // namespace

#endif
