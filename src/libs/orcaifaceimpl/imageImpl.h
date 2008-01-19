/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_IMPL_H
#define ORCA_IMAGE_IMPL_H

#include <orca/image.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <hydroiceutil/store.h>
#include <orcaice/context.h>

namespace hydroiceutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the orca::Image interface. Handles remote calls.
//!
class ImageImpl : public IceUtil::Shared
{
friend class ImageI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    ImageImpl( const orca::ImageDescription &descr,
               const std::string            &interfaceTag, 
               const orcaice::Context       &context );
    //! constructor using interfaceName
    ImageImpl( const orca::ImageDescription &descr,
               const orcaice::Context       &context,
               const std::string            &interfaceName );
    ~ImageImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroiceutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::ImageDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::ImageDataPtr& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::ImageDataPtr internalGetData() const;
    ::orca::ImageDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::ImageConsumerPrx&);
    void internalUnsubscribe(const ::orca::ImageConsumerPrx&);

    orca::ImageDescription     descr_;
    // yes, this is not a typo! It's safe to use a normal proxy with an Ice smart pointer.
    hydroiceutil::Store<orca::ImageDataPtr> dataStore_;

    orca::ImageConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx        topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              interfaceName_;
    const std::string              topicName_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<ImageImpl> ImageImplPtr;

}

#endif
