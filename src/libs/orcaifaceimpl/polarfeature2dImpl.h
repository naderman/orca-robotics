/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IFACEIMPL_POLARFEATURE2D_IMPL_H
#define ORCA_IFACEIMPL_POLARFEATURE2D_IMPL_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/polarfeature2d.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxsickacfr { namespace gbxiceutilacfr { class Thread; } }

namespace orcaifaceimpl {

//!
//! Implements the interface
//!
class PolarFeature2dImpl : public IceUtil::Shared
{
friend class PolarFeature2dI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    PolarFeature2dImpl( const orca::PolarFeature2dDescription &descr,
                        const std::string                     &interfaceTag,
                        const orcaice::Context                &context );
    //! constructor using interfaceName
    PolarFeature2dImpl( const orca::PolarFeature2dDescription &descr,
                        const orcaice::Context                &context,
                        const std::string                     &interfaceName );                        
    ~PolarFeature2dImpl();

    //
    // Local calls:
    //
    //! may throw gbxsickacfr::gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxsickacfr::gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PolarFeature2dData &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::PolarFeature2dData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PolarFeature2dData        internalGetData() const;
    ::orca::PolarFeature2dDescription internalGetDescription() const;
    void internalSubscribe(const ::orca::PolarFeature2dConsumerPrx&);
    void internalUnsubscribe(const ::orca::PolarFeature2dConsumerPrx&);

    // Holds the latest data
    gbxsickacfr::gbxiceutilacfr::Store<orca::PolarFeature2dData> dataStore_;

    orca::PolarFeature2dDescription descr_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::PolarFeature2dConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string                     interfaceName_;
    const std::string                     topicName_;
    orcaice::Context                context_;
};
typedef IceUtil::Handle<PolarFeature2dImpl> PolarFeature2dImplPtr;

} // namespace

#endif
