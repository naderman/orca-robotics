/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAIFACEIMPL_LOCALISE2D_IFACE_H
#define ORCA2_ORCAIFACEIMPL_LOCALISE2D_IFACE_H

#include <orca/localise2d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the Localise2d interface: Handles remote calls.
//!
class Localise2dIface : public IceUtil::Shared
{
friend class Localise2dI;

public:
    //! Constructor
    Localise2dIface( const std::string &ifaceTag,
                     const orcaice::Context &context );
    ~Localise2dIface();

    // Local calls:
    //! may throw orcaice::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Localise2dData &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Localise2dData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::Localise2dData getData() const;
    void subscribe(const ::orca::Localise2dConsumerPrx& );
    void unsubscribe(const ::orca::Localise2dConsumerPrx& );
    // Note: could do smoothing/interpolation here...
    orca::Localise2dData getDataAtTime(const orca::Time&) const
        { return getData(); }

    orcaice::Proxy<orca::Localise2dData> dataProxy_;

    orca::Localise2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string                 ifaceTag_;
    orcaice::Context                  context_;
};
typedef IceUtil::Handle<Localise2dIface> Localise2dIfacePtr;

} // namespace

#endif
