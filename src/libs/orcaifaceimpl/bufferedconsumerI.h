/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_BUFFERED_CONSUMER_I_H
#define ORCAIFACEIMPL_BUFFERED_CONSUMER_I_H

#include <orcaice/buffer.h>

namespace orcaifaceimpl
{

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Does nothing more than sticking incoming data into an orcaice::Buffer.
//!
template<class ConsumerType, class ObjectType>
class BufferedConsumerI : public ConsumerType
{
public:
    BufferedConsumerI() : 
        buffer_(-1,orcaice::BufferTypeCircular) {};

    // from ConsumerType
    virtual void setData( const ObjectType& data, const Ice::Current& )
    {
        buffer_.push( data );
    }

    orcaice::Buffer<ObjectType> &buffer() { return buffer_; }

private:
    orcaice::Buffer<ObjectType> buffer_;
};

} // namespace

#endif
