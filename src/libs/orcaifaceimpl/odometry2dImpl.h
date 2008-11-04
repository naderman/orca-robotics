/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ODOMETRY2D_IMPL_H
#define ORCA_ODOMETRY2D_IMPL_H

#include <orca/odometry2d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl {

//!
//! Implements the orca::Odometry2d interface: Handles remote calls.
//!
class Odometry2dImpl : public IceUtil::Shared
{
friend class Odometry2dI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    Odometry2dImpl( const orca::VehicleDescription &descr,
                    const std::string              &interfaceTag, 
                    const orcaice::Context         &context );
    //! constructor using interfaceName
    Odometry2dImpl( const orca::VehicleDescription &descr,
                    const orcaice::Context         &context,
                    const std::string              &interfaceName );
    ~Odometry2dImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Odometry2dData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Odometry2dData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::Odometry2dData internalGetData() const;
    ::orca::VehicleDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::Odometry2dConsumerPrx&);
    void internalUnsubscribe(const ::orca::Odometry2dConsumerPrx&);

    orca::VehicleDescription     descr_;
    gbxiceutilacfr::Store<orca::Odometry2dData> dataStore_;

    // IceStorm proxies
    orca::Odometry2dConsumerPrx    publisherPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr                 ptr_;

    const std::string              interfaceName_;
    const std::string              topicName_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<Odometry2dImpl> Odometry2dImplPtr;

}

#endif
