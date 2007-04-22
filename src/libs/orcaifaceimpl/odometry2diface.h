/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY2D_IFACE_H
#define ORCA2_ODOMETRY2D_IFACE_H

#include <orca/odometry2d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Odometry2d interface: Handles remote calls.
//!
class Odometry2dIface : public IceUtil::Shared
{
public:
    //! constructor
    Odometry2dIface( const orca::VehicleDescription& descr,
                     const std::string& ifaceTag, 
                     const orcaice::Context& context );
    ~Odometry2dIface();

    // remote interface

    ::orca::Odometry2dData getData() const;

    ::orca::VehicleDescription getDescription() const;

    void subscribe(const ::orca::Odometry2dConsumerPrx&);

    void unsubscribe(const ::orca::Odometry2dConsumerPrx&);


    // local interface:

    //! May throw orcaice::Exceptions.
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Odometry2dData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Odometry2dData& data );

private:

    orca::VehicleDescription     descr_;
    orcaice::Proxy<orca::Odometry2dData> dataProxy_;

    orca::Odometry2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr                 ptr_;

    const std::string              tag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<Odometry2dIface> Odometry2dIfacePtr;

}

#endif
