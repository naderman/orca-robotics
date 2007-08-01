/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PROXIED_CONSUMER_H
#define ORCAIFACEIMPL_PROXIED_CONSUMER_H

#include <orcaice/proxy.h>
#include <orcaifaceimpl/consumer.h>

namespace orcaifaceimpl
{

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Does nothing more than sticking incoming data into an orcaice::Proxy.
//!
//  Note: inheriting from IceUtil::Shared allows us to use Ice smart
//  pointers with these things.
template<class ConsumerType, class ConsumerPrxType, class ObjectType>
class ProxiedConsumer : public Consumer<ConsumerType,ConsumerPrxType,ObjectType>,
                        public IceUtil::Shared
{
public:

    ProxiedConsumer( const orcaice::Context &context )
        : Consumer<ConsumerType,ConsumerPrxType,ObjectType>(context) {}

    void setData( const ObjectType& data ) { proxy_.set( data ); }

    //! Returns reference to local proxy.
    orcaice::Proxy<ObjectType> &proxy() { return proxy_; }

private:
    orcaice::Proxy<ObjectType> proxy_;
};

} // namespace

#endif
