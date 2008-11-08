/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_STORING_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_STORING_CONSUMER_IMPL_H

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaifaceimpl/consumerImpl.h>

namespace orcaifaceimpl
{

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Does nothing more than sticking incoming data into an gbxiceutilacfr::Store.
//!
template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class StoringConsumerImpl : 
        public ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>
{
public:
    //! Constructor.
    StoringConsumerImpl( const orcaice::Context &context )
        : ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context) {}

    //! Returns reference to local store.
    gbxiceutilacfr::Store<ObjectType> &store() { return store_; }

    //! This callback simply puts the data object into the internal store.
    virtual void dataEvent( const ObjectType& data ) 
    {
        store_.set( data ); 
    }

private:
    gbxiceutilacfr::Store<ObjectType> store_;
};

} // namespace

#endif
