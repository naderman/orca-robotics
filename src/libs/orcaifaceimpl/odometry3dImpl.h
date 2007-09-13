/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY3D_IMPL_H
#define ORCA2_ODOMETRY3D_IMPL_H

#include <orca/odometry3d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Odometry3d interface: Handles remote calls.
//!
class Odometry3dImpl : public IceUtil::Shared
{
friend class Odometry3dI;

public:
    //! constructor
    Odometry3dImpl( const orca::VehicleDescription& descr,
                 const std::string& interfaceTag, 
                 const orcaice::Context& context );
    ~Odometry3dImpl();

    // local interface:
    //! May throw orcaice::Exceptions.
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Odometry3dData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Odometry3dData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::Odometry3dData internalGetData() const;
    ::orca::VehicleDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::Odometry3dConsumerPrx&);
    void internalUnsubscribe(const ::orca::Odometry3dConsumerPrx&);

    orca::VehicleDescription     descr_;
    orcaice::Proxy<orca::Odometry3dData> dataProxy_;

    orca::Odometry3dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              tag_;
    orcaice::Context               context_;
};
typedef IceUtil::Handle<Odometry3dImpl> Odometry3dImplPtr;

}

#endif