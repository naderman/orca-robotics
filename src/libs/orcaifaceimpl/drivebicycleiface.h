/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAIFACEIMPL_DRIVEBICYCLE_IFACE_H
#define ORCA2_ORCAIFACEIMPL_DRIVEBICYCLE_IFACE_H

#include <orca/drivebicycle.h>
#include <IceStorm/IceStorm.h>

#include <orcaice/proxy.h>
#include <orcaice/notify.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the DriveBicycle interface: Handles remote calls.
//!
class DriveBicycleIface : public IceUtil::Shared,
                          public orcaice::Notify<orca::DriveBicycleCommand>
{
friend class DriveBicycleI;

public:
    //! Constructor
    DriveBicycleIface( const orca::VehicleDescription& descr,
                       const std::string &ifaceTag,
                       const orcaice::Context &context );
    ~DriveBicycleIface();

    // Local calls:
    //! may throw orcaice::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::DriveBicycleData &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::DriveBicycleData &data );

private:

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::DriveBicycleData getData() const;
    void subscribe(const ::orca::DriveBicycleConsumerPrx& );
    void unsubscribe(const ::orca::DriveBicycleConsumerPrx& );
    orca::VehicleDescription getDescription() const
        { return description_; }
    void setCommand( const ::orca::DriveBicycleCommand& );

    const orca::VehicleDescription    description_;
    const std::string                 ifaceTag_;
    IceStorm::TopicPrx                topicPrx_;

    // outgoing data
    orcaice::Proxy<orca::DriveBicycleData> dataPipe_;

    orcaice::Context                  context_;

    orca::DriveBicycleConsumerPrx     consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;
};
typedef IceUtil::Handle<DriveBicycleIface> DriveBicycleIfacePtr;

} // namespace

#endif
