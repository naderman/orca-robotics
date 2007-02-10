/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAIFACEIMPL_LOCALISE3D_I_H
#define ORCA2_ORCAIFACEIMPL_LOCALISE3D_I_H

#include <orca/localise3d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the Localise3d interface: Handles remote calls.
//!
class Localise3dI : public orca::Localise3d
{
public:
    Localise3dI( const std::string &ifaceTag,
                 const orcaice::Context &context );

    // remote calls:

    virtual ::orca::Localise3dData getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Note: could do smoothing/interpolation here...
    virtual orca::Localise3dData getDataAtTime(const orca::Time&, const Ice::Current& c) const
        { return getData(c); }

    // Local calls:

    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const orca::Localise3dData &data );

private:

    orcaice::Proxy<orca::Localise3dData> dataProxy_;

    orca::Localise3dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    const std::string              ifaceTag_;
    orcaice::Context               context_;
};

} // namespace

#endif
