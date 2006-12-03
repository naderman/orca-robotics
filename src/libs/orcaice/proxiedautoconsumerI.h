/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAICE_PROXIED_AUTO_CONSUMER_I_H
#define ORCAICE_PROXIED_AUTO_CONSUMER_I_H

#include <orcaice/autoconsumerI.h>
#include <orcaice/ptrproxy.h>

namespace orcaice
{

/*
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does one thing: puts it into its internal proxy.
 *
 *  Relies on the fact that the Consumer objects have only one operation
 *  to implement and it's called setData().
 *
 *  @see BufferedAutoConsumerI
 */
template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
class ProxiedAutoConsumerI : public AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>
{
public:
    ProxiedAutoConsumerI( const orcaice::Context & context, const std::string & tag );
    
    virtual ~ProxiedAutoConsumerI() {};
    
    // buffer_ is public so that guielements can access it directly
    orcaice::PtrProxy<ObjectPtrType> proxy_;

protected:

    virtual void storeData( const ObjectPtrType & obj );
};

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
ProxiedAutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::ProxiedAutoConsumerI
                        ( const orcaice::Context & context, const std::string & tag )
    : AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>(context,tag)
{
}

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
void
ProxiedAutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::storeData( const ObjectPtrType & obj )
{
    proxy_.set( obj );
}

} // namespace

#endif
