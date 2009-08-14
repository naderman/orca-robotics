/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PRINTING_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_PRINTING_CONSUMER_IMPL_H

#include <orcaifaceimpl/consumerImpl.h>

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
// template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
template<class ProviderType, class ConsumerType, class ObjectType>
class PrintingConsumerImpl : 
        public ConsumerImpl<ProviderType,ConsumerType,ObjectType>
//         public ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>
{
// using ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>::context_;
using ConsumerImpl<ProviderType,ConsumerType,ObjectType>::context_;
public:
    //! Constructor. The optinal parameter @c recurse specifies the level of detail printed
    //! out to @c cout when new data arrives.
    PrintingConsumerImpl( const orcaice::Context &context, int recurse=1000, int expand=-1 ) :
        ConsumerImpl<ProviderType,ConsumerType,ObjectType>(context), 
//         ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context), 
        recurse_(recurse),
        expand_(expand) {}

    //! This callback simply prints it out to standard output.
    virtual void dataEvent( const ObjectType& data ) 
    {
        std::cout << "PrintingConsumerImpl::dataEvent() start" << std::endl;
        context_.tracer().info( ifaceutil::toString(data,recurse_,expand_) ); 
    }
private:
    int recurse_;
    int expand_;
};

} // namespace

#endif
