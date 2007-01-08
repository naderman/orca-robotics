/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POWER_I_H
#define ORCA2_POWER_I_H

#include <orca/power.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Power interface: Handles remote calls.
//!
class PowerI : public orca::Power
{
public:
    //! constructor
    PowerI( const std::string& ifaceTag, 
            const orcaice::Context& context );
    virtual ~PowerI();

    // remote interface

    virtual ::orca::PowerData getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    // local interface:

    //! May throw orcaice::Exceptions.
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::PowerData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::PowerData& data );

private:

    orcaice::Proxy<orca::PowerData> dataProxy_;

    orca::PowerConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    const std::string              tag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<PowerI> PowerIPtr;

}

#endif
