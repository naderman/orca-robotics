/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CAMERA_I_H
#define ORCA2_CAMERA_I_H

#include <orca/camera.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Camera interface. Handles remote calls.
//!
class CameraI : public orca::Camera
{
public:
    //! constructor
    CameraI( const orca::CameraDescription& descr,
                 const std::string& ifaceTag, 
                 const orcaice::Context& context );
    virtual ~CameraI();

    // remote interface

    virtual ::orca::CameraData getData(const ::Ice::Current& ) const;

    virtual ::orca::CameraDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    // local interface:

    //! May throw orcaice::Exceptions.
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::CameraData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::CameraData& data );

private:

    orca::CameraDescription     descr_;
    orcaice::Proxy<orca::CameraData> dataProxy_;

    orca::CameraConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    const std::string              tag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<CameraI> CameraIPtr;

}

#endif
