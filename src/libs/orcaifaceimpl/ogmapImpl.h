/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef __ogmapIface_h__
#define __ogmapIface_h__

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogmap.h>

#include <hydroutil/store.h>
#include <orcaice/context.h>

namespace hydroutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the OgMap interface: Handles remote calls.
//!
class OgMapImpl : public IceUtil::Shared
{
friend class OgMapI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    OgMapImpl( const std::string      &interfaceTag,
               const orcaice::Context &context );
    //! constructor using interfaceName
    OgMapImpl( const orcaice::Context &context,
               const std::string      &interfaceName );               
    ~OgMapImpl();

    // Local calls:
    //! Sets up interface and connects to IceStorm. May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::OgMapData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::OgMapData internalGetData() const;
    void internalSubscribe(const ::orca::OgMapConsumerPrx& );
    void internalUnsubscribe(const ::orca::OgMapConsumerPrx& );

    // Holds the latest data
    hydroutil::Store<orca::OgMapData> dataStore_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::OgMapConsumerPrx         consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string interfaceName_;
    const std::string topicName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<OgMapImpl> OgMapImplPtr;

}

#endif
