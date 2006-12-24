/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAICE_PTR_PROXIED_CONSUMER_I_H
#define ORCAICE_PTR_PROXIED_CONSUMER_I_H

#include <orcaice/ptrproxy.h>

namespace orcaice
{

/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does one thing: puts it into its internal proxy.
 *
 *  Relies on the fact that the Consumer objects have only one operation
 *  to implement and it's called setData().
 *
 *  @see PtrProxiedConsumerI
 */
template<class ConsumerType, class ObjectPtrType>
class PtrProxiedConsumerI : public ConsumerType
{
public:

    virtual ~PtrProxiedConsumerI() {};
    
    virtual void setData( const ObjectPtrType& data, const Ice::Current& );

    // proxy_ is public so that guielements can access it directly
    orcaice::PtrProxy<ObjectPtrType> proxy_;

protected:

    //! You can derive from this class and do something when an object is received.
    //! (besides sticking it into the proxy). This adds the functionality of orcaice::PtrNotify.
    //! The default implementation does nothing.
    virtual void handleData( const ObjectPtrType & obj ) {};
};

template<class ConsumerType, class ObjectPtrType>
void
PtrProxiedConsumerI<ConsumerType,ObjectPtrType>::setData( const ObjectPtrType& data, const Ice::Current& )
{
    // stick data into the internal proxy
    proxy_.set( data );

    // perform optional data handling implemented in derived classes
    handleData( data );
}

} // namespace

#endif
