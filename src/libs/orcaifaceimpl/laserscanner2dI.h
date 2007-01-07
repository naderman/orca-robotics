/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_LASERSCANNER2D_I_H
#define ORCA2_LASER2D_LASERSCANNER2D_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/laserscanner2d.h>

// utilities
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the Localise2d interface: Handles remote calls.
//!
class LaserScanner2dI : public virtual orca::LaserScanner2d
{
public:
    LaserScanner2dI( const orca::RangeScanner2dDescription &descr,
                     const std::string                     &ifaceTag,
                     const orcaice::Context                &context );

    //
    // Remote calls:
    //

    virtual ::orca::RangeScanner2dDataPtr     getData(const ::Ice::Current& ) const;

    virtual ::orca::RangeScanner2dDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::RangeScanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::RangeScanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    //
    // Local calls:
    //

    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const ::orca::LaserScanner2dDataPtr &data );

private:

    // Holds the latest data
    orcaice::PtrProxy<orca::LaserScanner2dDataPtr> dataProxy_;

    orca::RangeScanner2dDescription descr_;

    // The topic to which we'll publish
    IceStorm::TopicPrx              topicPrx_;
    // The interface to which we'll publish
    orca::RangeScanner2dConsumerPrx consumerPrx_;

    std::string                     ifaceTag_;
    orcaice::Context                context_;
};

} // namespace

#endif
