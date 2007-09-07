/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PROXIED_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_PROXIED_CONSUMER_IMPL_H

#include <orcaice/proxy.h>
#include <orcaifaceimpl/consumerImpl.h>

namespace orcaifaceimpl
{

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Does nothing more than sticking incoming data into an orcaice::Proxy.
//!
//  Note: inheriting from IceUtil::Shared allows us to use Ice smart
//  pointers with these things.
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class ProxiedConsumerImpl : 
        public ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>,
        public IceUtil::Shared
{
public:
    //! Constructor.
    ProxiedConsumerImpl( const orcaice::Context &context )
        : ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context) {}

    //! Returns reference to local proxy.
    orcaice::Proxy<ObjectType> &proxy() { return proxy_; }

    //! This callback simply puts the data object into the internal proxy.
    virtual void handleData( const ObjectType& data ) { proxy_.set( data ); }

private:
    orcaice::Proxy<ObjectType> proxy_;
};

} // namespace

#endif
