/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ESTOP_IMPL_H
#define ORCA_ESTOP_IMPL_H

#include <hydroiceutil/store.h>
#include <orcaice/context.h>

#include <orca/estop.h>
#include <IceStorm/IceStorm.h>

// utilities


namespace orcaifaceimpl {



//!
//! Implements the orca::EStop interface: Handles remote calls.
//! Intended that the consumer of this interface should, take the
//! appropriate action and 'fail-safe' if data is not being received
//! across the interface. Or if data received indicates a fault condition
//!

class EStopImpl : public IceUtil::Shared
{
friend class EStopI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    EStopImpl( const std::string& interfaceTag, 
               const orcaice::Context& context );
    //! constructor using interfaceName
    EStopImpl( const orcaice::Context& context,
               const std::string& interfaceName );
    ~EStopImpl();

    // local interface:
    //! May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroiceutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the state reported by the interface
    void localSet( const orca::EStopData data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::EStopData data );

private:

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::EStopData internalGet() const;
    void internalSubscribe(const ::orca::EStopConsumerPrx&);
    void internalUnsubscribe(const ::orca::EStopConsumerPrx&);


    hydroiceutil::Store<orca::EStopData> dataStore_;

    orca::EStopConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx        topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              interfaceName_;
    const std::string              topicName_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<EStopImpl> EStopImplPtr;


}// namespace

#endif
