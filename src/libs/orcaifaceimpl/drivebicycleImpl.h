/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAIFACEIMPL_DRIVEBICYCLE_IMPL_H
#define ORCA_ORCAIFACEIMPL_DRIVEBICYCLE_IMPL_H

#include <memory>
#include <orca/drivebicycle.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/notify.h>
#include <orcaice/context.h>
#include <orcaice/topichandler.h>

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
    void init();

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::DriveBicycleData internalGetData() const;
    IceStorm::TopicPrx internalSubscribe(const orca::DriveBicycleConsumerPrx& subscriber);
    orca::VehicleDescription internalGetDescription() const
        { return description_; }
    void internalSetCommand( const ::orca::DriveBicycleCommand& );

    const orca::VehicleDescription    description_;
    gbxiceutilacfr::Store<orca::DriveBicycleData> dataStore_;

    typedef orcaice::TopicHandler<orca::DriveBicycleConsumerPrx,orca::DriveBicycleData> DriveBicycleTopicHandler;
    std::auto_ptr<DriveBicycleTopicHandler> topicHandler_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<DriveBicycleImpl> DriveBicycleImplPtr;

} // namespace

#endif
