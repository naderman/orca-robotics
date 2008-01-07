/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PROPERTIES_IMPL_H
#define ORCA_PROPERTIES_IMPL_H

#include <orca/properties.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <hydroiceutil/store.h>
#include <orcaice/context.h>

namespace hydroiceutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the orca::Properties interface: Handles remote calls.
//!
class PropertiesImpl : public IceUtil::Shared
{
friend class PropertiesI;

public:
    //! constructor using interfaceTag (may throw ConfigFileException)
    PropertiesImpl( const std::string& interfaceTag, 
               const orcaice::Context& context );
    //! constructor using interfaceName
    PropertiesImpl( const orcaice::Context& context,
               const std::string& interfaceName );
    ~PropertiesImpl();

    // local interface:
    //! May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroiceutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PropertiesData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::PropertiesData& data );

    //! A local call which gets the Store that holds remote 'setData' requests.
    hydroiceutil::Store<orca::PropertiesData> &localGetRemotelySetDataStore()
        { return remotelySetDataStore_; }

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PropertiesData internalGetData() const;
    void internalSetData( const ::orca::PropertiesData &data );
    void internalSubscribe(const ::orca::PropertiesConsumerPrx&);
    void internalUnsubscribe(const ::orca::PropertiesConsumerPrx&);

    hydroiceutil::Store<orca::PropertiesData> dataStore_;
    hydroiceutil::Store<orca::PropertiesData> remotelySetDataStore_;

    orca::PropertiesConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              interfaceName_;
    const std::string              topicName_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<PropertiesImpl> PropertiesImplPtr;

}

#endif
