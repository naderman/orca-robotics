/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IFACEIMPL_POLARFEATURE2D_IMPL_H
#define ORCA2_IFACEIMPL_POLARFEATURE2D_IMPL_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/polarfeature2d.h>

// utilities
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the interface
//!
class PolarFeature2dImpl : public IceUtil::Shared
{
friend class PolarFeature2dI;

public:
    //! Constructor
    PolarFeature2dImpl( const std::string                     &interfaceTag,
                         const orcaice::Context                &context );
    ~PolarFeature2dImpl();

    //
    // Local calls:
    //
    //! may throw orcaiceutil::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PolarFeature2dDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::PolarFeature2dDataPtr &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PolarFeature2dDataPtr     internalGetData() const;
    void internalSubscribe(const ::orca::PolarFeature2dConsumerPrx&);
    void internalUnsubscribe(const ::orca::PolarFeature2dConsumerPrx&);

    // Holds the latest data
    orcaice::PtrProxy<orca::PolarFeature2dDataPtr> dataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::PolarFeature2dConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    std::string                     interfaceTag_;
    orcaice::Context                context_;
};
typedef IceUtil::Handle<PolarFeature2dImpl> PolarFeature2dImplPtr;

} // namespace

#endif
