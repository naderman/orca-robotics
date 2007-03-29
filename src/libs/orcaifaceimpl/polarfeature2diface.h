/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IFACEIMPL_POLARFEATURE2D_IFACE_H
#define ORCA2_IFACEIMPL_POLARFEATURE2D_IFACE_H

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
class PolarFeature2dIface : public IceUtil::Shared
{
public:
    PolarFeature2dIface( const std::string                     &ifaceTag,
                         const orcaice::Context                &context );
    ~PolarFeature2dIface();

    //
    // Remote calls:
    //

    ::orca::PolarFeature2dDataPtr     getData() const;

    void subscribe(const ::orca::PolarFeature2dConsumerPrx&);

    void unsubscribe(const ::orca::PolarFeature2dConsumerPrx&);


    //
    // Local calls:
    //

    //! may throw orcaice::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PolarFeature2dDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::PolarFeature2dDataPtr &data );

private:

    // Holds the latest data
    orcaice::PtrProxy<orca::PolarFeature2dDataPtr> dataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::PolarFeature2dConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    std::string                     ifaceTag_;
    orcaice::Context                context_;
};
typedef IceUtil::Handle<PolarFeature2dIface> PolarFeature2dIfacePtr;

} // namespace

#endif
