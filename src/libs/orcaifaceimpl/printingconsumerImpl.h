/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PRINTING_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_PRINTING_CONSUMER_IMPL_H

#include <orcaifaceimpl/consumerImpl.h>
#include <orcaobj/orcaobj.h>

namespace orcaifaceimpl
{

/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does one thing: puts out data as text through
 *  as an info message of Tracer.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData() and toString() function is implemented
 *  for the data type.
 */
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class PrintingConsumerImpl : 
        public ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>
{
using ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>::context_;
public:
    //! Constructor.
    PrintingConsumerImpl( const orcaice::Context &context )
        : ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context) {}

    //! This callback simply prints it out to standard output.
    virtual void handleData( const ObjectType& data ) 
    {
        context_.tracer().info( orcaobj::toString(data) ); 
    }
};

} // namespace

#endif
