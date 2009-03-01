/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAIFACEIMPL_NOTIFYINGCONSUMERIMPL_H
#define ORCAIFACEIMPL_NOTIFYINGCONSUMERIMPL_H

#include <gbxsickacfr/gbxiceutilacfr/notify.h>
#include <orcaifaceimpl/consumerImpl.h>
#include <iostream>

namespace orcaifaceimpl {

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Users must register a callback which is notified on arrival of data.
//!
template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class NotifyingConsumerImpl
    : public ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>,
      public gbxiceutilacfr::Notify<ObjectType>
{

public: 

    //! Constructor.
    NotifyingConsumerImpl( const orcaice::Context &context )
        : ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context) {}

    ~NotifyingConsumerImpl()
        {
            std::cout<<"NotifyingConsumerImpl::~NotifyingConsumerImpl()"<<std::endl;
        }

    // from gbxiceutilacfr::Notify:
    // void  setNotifyHandler( NotifyHandler<Type>* handler );

    // from gbxiceutilacfr::Notify (calls handler->handleData( const Type & ))
    // bool hasNotifyHandler() { return hasNotifyHandler_; };

private:

    // inherited from ConsumerImpl
    virtual void dataEvent( const ObjectType &data )
        {
            // results in handler->handleData(data) being called
            set( data );
        }
};

}

#endif
