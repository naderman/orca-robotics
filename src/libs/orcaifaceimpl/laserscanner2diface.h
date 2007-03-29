/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_LASERSCANNER2D_IFACE_H
#define ORCA2_LASER2D_LASERSCANNER2D_IFACE_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/laserscanner2d.h>

// utilities
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Sets up an implementation of the remote interface, and handles remote calls.
//!
class LaserScanner2dIface : public IceUtil::Shared
{
public:
    LaserScanner2dIface( const orca::RangeScanner2dDescription &descr,
                         const std::string                     &ifaceTag,
                         const orcaice::Context                &context );

    ~LaserScanner2dIface();

    //
    // Remote calls:
    //

    ::orca::RangeScanner2dDataPtr     getData() const;

    ::orca::RangeScanner2dDescription getDescription() const;

    void subscribe(const ::orca::RangeScanner2dConsumerPrx& );

    void unsubscribe(const ::orca::RangeScanner2dConsumerPrx& );


    //
    // Local calls:
    //

    //! may throw orcaice::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::LaserScanner2dDataPtr& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const ::orca::LaserScanner2dDataPtr &data );

private:

    // Holds the latest data
    orcaice::PtrProxy<orca::LaserScanner2dDataPtr> dataProxy_;

    orca::RangeScanner2dDescription descr_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::RangeScanner2dConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    std::string        ifaceTag_;
    orcaice::Context   context_;
};
typedef IceUtil::Handle<LaserScanner2dIface> LaserScanner2dIfacePtr;

} // namespace

#endif
