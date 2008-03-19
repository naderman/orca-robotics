/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMU_IMPL_H
#define ORCA_IMU_IMPL_H

#include <orca/imu.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxsickacfr { namespace gbxiceutilacfr { class Thread; } }

namespace orcaifaceimpl {

//!
//! Implements the orca::Imu interface: Handles remote calls.
//!
class ImuImpl : public IceUtil::Shared
{
friend class ImuI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    ImuImpl( const orca::ImuDescription &descr,
                    const std::string              &interfaceTag, 
                    const orcaice::Context         &context );
    //! constructor using interfaceName
    ImuImpl( const orca::ImuDescription &descr,
                    const orcaice::Context         &context,
                    const std::string              &interfaceName );
    ~ImuImpl();

    // local interface:
    //! May throw gbxsickacfr::gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxsickacfr::gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::ImuData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::ImuData& data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::ImuData internalGetData() const;
    ::orca::ImuDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::ImuConsumerPrx&);
    void internalUnsubscribe(const ::orca::ImuConsumerPrx&);

    orca::ImuDescription     descr_;
    gbxsickacfr::gbxiceutilacfr::Store<orca::ImuData> dataStore_;

    orca::ImuConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string       interfaceName_;
    const std::string       topicName_;
    orcaice::Context        context_;
};
typedef IceUtil::Handle<ImuImpl> ImuImplPtr;

}

#endif
