/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_NOTIFIED_EXTERNAL_CONSUMER_I_H
#define ORCAIFACEIMPL_NOTIFIED_EXTERNAL_CONSUMER_I_H

#include <orcaice/notify.h>

namespace orcaifaceimpl
{

/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does one thing: calls the notify callback function.
 *  In this implementation, the Notify object is external and is set in the constructor.
 *
 *  Relies on the fact that the Consumer objects have only one operation
 *  to implement and it's called setData().
 *
 */
template<class ConsumerType, class ObjectType>
class NotifiedExtConsumerI : public ConsumerType
{
public:
    NotifiedExtConsumerI( orcaice::Notify<ObjectType>& notify ) :
        notify_(notify) {};

    virtual ~NotifiedExtConsumerI() {};
    
    virtual void setData( const ObjectType& data, const Ice::Current& );

private:
    orcaice::Notify<ObjectType>& notify_;
};

template<class ConsumerType, class ObjectType>
void
NotifiedExtConsumerI<ConsumerType,ObjectType>::setData( const ObjectType& data, const Ice::Current& )
{
    notify_.set( data );
}

} // namespace

#endif
