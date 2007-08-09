/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_IFACE_H
#define ORCA2_IMAGE_IFACE_H

#include <orca/image.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaice {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the orca::Image interface. Handles remote calls.
//!
class ImageIface : public IceUtil::Shared
{
friend class ImageI;

public:
    //! Constructor
    ImageIface( const orca::ImageDescription& descr,
                 const std::string& ifaceTag, 
                 const orcaice::Context& context );
    ~ImageIface();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw orcaice::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( orcaice::Thread* thread, int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::ImageDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::ImageDataPtr& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::ImageDataPtr getData() const;
    ::orca::ImageDescription getDescription() const;
    void subscribe(const ::orca::ImageConsumerPrx&);
    void unsubscribe(const ::orca::ImageConsumerPrx&);

    orca::ImageDescription     descr_;
    // yes, this is not a typo! It's safe to use a normal proxy with an Ice smart pointer.
    orcaice::Proxy<orca::ImageDataPtr> dataProxy_;

    orca::ImageConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx        topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              ifaceTag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<ImageIface> ImageIfacePtr;

}

#endif
