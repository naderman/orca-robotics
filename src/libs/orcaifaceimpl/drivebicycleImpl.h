/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAIFACEIMPL_DRIVEBICYCLE_IMPL_H
#define ORCA_ORCAIFACEIMPL_DRIVEBICYCLE_IMPL_H

#include <orca/drivebicycle.h>
#include <IceStorm/IceStorm.h>

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/notify.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Implements the DriveBicycle interface: Handles remote calls.
//!
class DriveBicycleImpl : public IceUtil::Shared,
                          public gbxiceutilacfr::Notify<orca::DriveBicycleCommand>
{
friend class DriveBicycleI;

public:

    //! constructor using interfaceTag (may throw ConfigFileException)
    DriveBicycleImpl( const orca::VehicleDescription& descr,
                      const std::string &interfaceTag,
                      const orcaice::Context &context );
    //! constructor using interfaceName
    DriveBicycleImpl( const orca::VehicleDescription& descr,
                      const orcaice::Context &context,
                      const std::string &interfaceName );
    ~DriveBicycleImpl();

    // Local calls:
    //! Sets up interface and connects to IceStorm. May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::DriveBicycleData &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::DriveBicycleData &data );

private:

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::DriveBicycleData internalGetData() const;
    void internalSubscribe(const ::orca::DriveBicycleConsumerPrx& );
    void internalUnsubscribe(const ::orca::DriveBicycleConsumerPrx& );
    orca::VehicleDescription internalGetDescription() const
        { return description_; }
    void internalSetCommand( const ::orca::DriveBicycleCommand& );

    const orca::VehicleDescription    description_;
    const std::string                 interfaceName_;
    const std::string                 topicName_;
    IceStorm::TopicPrx                topicPrx_;

    // outgoing data
    gbxiceutilacfr::Store<orca::DriveBicycleData> dataPipe_;

    orcaice::Context                  context_;

    orca::DriveBicycleConsumerPrx     publisherPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;
};
typedef IceUtil::Handle<DriveBicycleImpl> DriveBicycleImplPtr;

} // namespace

#endif
