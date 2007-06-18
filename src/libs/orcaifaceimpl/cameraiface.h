/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CAMERA_IFACE_H
#define ORCA2_CAMERA_IFACE_H

#include <orca/camera.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Camera interface. Handles remote calls.
//!
class CameraIface : public IceUtil::Shared
{
friend class CameraI;

public:
    //! constructor
    CameraIface( const orca::CameraDescription& descr,
                 const std::string& ifaceTag, 
                 const orcaice::Context& context );
    ~CameraIface();

    // local interface:
    //! May throw orcaice::Exceptions.
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::CameraData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::CameraData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::CameraData getData() const;
    ::orca::CameraDescription getDescription() const;
    void subscribe(const ::orca::CameraConsumerPrx&);
    void unsubscribe(const ::orca::CameraConsumerPrx&);

    orca::CameraDescription     descr_;
    orcaice::Proxy<orca::CameraData> dataProxy_;

    orca::CameraConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              tag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<CameraIface> CameraIfacePtr;

}

#endif
