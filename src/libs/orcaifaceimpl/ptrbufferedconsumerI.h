/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PTR_BUFFERED_CONSUMER_I_H
#define ORCAIFACEIMPL_PTR_BUFFERED_CONSUMER_I_H

#include <orcaice/ptrbuffer.h>

namespace orcaifaceimpl
{

/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData().
 */
template<class ConsumerType, class ObjectPtrType>
class PtrBufferedConsumerI : public ConsumerType
{
public:

    PtrBufferedConsumerI( int depth=1, orcaice::BufferType bufferType = orcaice::BufferTypeCircular )
        : buffer_( depth, bufferType ) {};

    virtual void setData( const ObjectPtrType& data, const Ice::Current& );

    // buffer_ is public so that guielements can access it directly
    orcaice::PtrBuffer<ObjectPtrType> buffer_;

protected:

    //! You can derive from this class and do something when an object is received.
    //! (besides sticking it in the buffer). This adds the functionality of orcaice::PtrNotify.
    virtual void handleData( const ObjectPtrType & obj ) {};
};

template<class ConsumerType, class ObjectPtrType>
void
PtrBufferedConsumerI<ConsumerType,ObjectPtrType>::setData( const ObjectPtrType& data, const Ice::Current& )
{
    buffer_.push( data );
    handleData( data );
}

} // namespace

#endif
