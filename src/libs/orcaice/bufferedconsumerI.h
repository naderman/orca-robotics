/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAICE_BUFFERED_CONSUMER_I_H
#define ORCAICE_BUFFERED_CONSUMER_I_H

#include <orcaice/buffer.h>

namespace orcaice
{

/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData().
 *
 *  @see PtrBufferedConsumerI when ObjectType is an Ice smart pointer.
 */
template<class ConsumerType, class ObjectType>
class BufferedConsumerI : public ConsumerType
{
public:

    BufferedConsumerI( int depth=1, orcaice::BufferType bufferType = orcaice::BufferTypeCircular )
        : buffer_( depth, bufferType ) {};

    virtual void setData( const ObjectType& data, const Ice::Current& );

    // buffer_ is public so that guielements can access it directly
    orcaice::Buffer<ObjectType> buffer_;

protected:

    //! You can derive from this class and do something when an object is received.
    //! (besides sticking it in the buffer). This adds the functionality of orcaice::PtrNotify.
    virtual void handleData( const ObjectType & obj ) {};
};

template<class ConsumerType, class ObjectType>
void
BufferedConsumerI<ConsumerType,ObjectType>::setData( const ObjectType& data, const Ice::Current& )
{
    buffer_.push( data );
    handleData( data );
}

} // namespace

#endif
