/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_GPS_IMPL_H
#define ORCA_GPS_IMPL_H

#include <orca/gps.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <hydroutil/store.h>
#include <orcaice/context.h>

namespace hydroutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the orca::Gps interface: Handles remote calls.
//!
class GpsImpl : public IceUtil::Shared
{
friend class GpsI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    GpsImpl( const orca::GpsDescription &descr,
             const std::string              &interfaceTag, 
             const orcaice::Context         &context );
    //! constructor using interfaceName
    GpsImpl( const orca::GpsDescription &descr,
             const orcaice::Context         &context,
             const std::string              &interfaceName );
    ~GpsImpl();

    // local interface:
    //! Sets up interface and connects to IceStorm. May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::GpsData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::GpsData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::GpsData internalGetData() const;
    ::orca::GpsDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::GpsConsumerPrx&);
    void internalUnsubscribe(const ::orca::GpsConsumerPrx&);

    orca::GpsDescription     descr_;
    hydroutil::Store<orca::GpsData> dataStore_;

    // IceStorm proxies
    orca::GpsConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr                 ptr_;

    const std::string              interfaceName_;
    const std::string              topicName_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<GpsImpl> GpsImplPtr;

}

#endif
