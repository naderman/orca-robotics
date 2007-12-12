/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAIFACEIMPL_NOTIFYINGCONSUMERIMPL_H
#define ORCAIFACEIMPL_NOTIFYINGCONSUMERIMPL_H

#include <hydroutil/notify.h>
#include <orcaifaceimpl/consumerImpl.h>
#include <iostream>

namespace orcaifaceimpl {

//!
//! Implements a consumer interface whose set method is 'SetData'.
//! Users must register a callback which is notified on arrival of data.
//!
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class NotifyingConsumerImpl
    : public ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>,
      public hydroutil::Notify<ObjectType>
{

public: 

    //! Constructor.
    NotifyingConsumerImpl( const orcaice::Context &context )
        : ConsumerImpl<ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context) {}

    // from hydroutil::Notify:
    // void  setNotifyHandler( NotifyHandler<Type>* handler );

    // from hydroutil::Notify (calls handler->handleData( const Type & ))
    // bool hasNotifyHandler() { return hasNotifyHandler_; };

private:

    // inherited from ConsumerImpl
    void handleData( const ObjectType &data )
        {
            // results in mainThread_->handleData(data) being called
            set( data );
        }

};

}

#endif
