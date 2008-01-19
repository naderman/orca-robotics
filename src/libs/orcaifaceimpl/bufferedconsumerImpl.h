/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_BUFFERED_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_BUFFERED_CONSUMER_IMPL_H

#include <hydroiceutil/buffer.h>
#include <orcaifaceimpl/consumerImpl.h>

namespace orcaifaceimpl
{

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Does nothing more than sticking incoming data into an hydroiceutil::Buffer.
//!
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class BufferedConsumerImpl : 
        public ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>
{
public:
    //! Constructor.
    BufferedConsumerImpl( int depth, hydroiceutil::BufferType type, const orcaice::Context &context )
        : ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context),
          buffer_(depth,type) {}

    //! Returns reference to local proxy.
    hydroiceutil::Buffer<ObjectType> &buffer() { return buffer_; }

    //! This callback simply puts the data object into the internal proxy.
    virtual void handleData( const ObjectType& data ) { buffer_.push( data ); }

private:
    hydroiceutil::Buffer<ObjectType> buffer_;
};

} // namespace

#endif
