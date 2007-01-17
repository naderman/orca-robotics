/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY3D_I_H
#define ORCA2_ODOMETRY3D_I_H

#include <orca/odometry3d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Odometry3d interface: Handles remote calls.
//!
class Odometry3dI : public orca::Odometry3d
{
public:
    //! constructor
    Odometry3dI( const orca::VehicleDescription& descr,
                 const std::string& ifaceTag, 
                 const orcaice::Context& context );
    virtual ~Odometry3dI();

    // remote interface

    virtual ::orca::Odometry3dData getData(const ::Ice::Current& ) const;

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Odometry3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Odometry3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    // local interface:

    //! May throw orcaice::Exceptions.
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Odometry3dData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Odometry3dData& data );

private:

    orca::VehicleDescription     descr_;
    orcaice::Proxy<orca::Odometry3dData> dataProxy_;

    orca::Odometry3dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    const std::string              tag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<Odometry3dI> Odometry3dIPtr;

}

#endif
