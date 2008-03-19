/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_PTR_ICESTORM_LISTENER_H
#define ORCAGUI_PTR_ICESTORM_LISTENER_H

#include <iostream>
#include <assert.h>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaice/connectutils.h>
#include <orcaice/context.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <orcaqguielementutil/icestormlistenerdetail.h>

namespace orcaqguielementutil
{

////////////////////////////////////////////////////////////////////////////////

/*!
    \brief Listens to IceStorm and stores the incoming data.
    
    Handles stuff like maintaining time since last receipt of data.
    
    \author Alex Brooks
*/
template< class DataType,
          class DataPtrType,
          class ProxyType,
          class ConsumerType,
          class ConsumerPrxType,
          class SubscriptionMakerType=detail::DefaultSubscriptionMaker<ProxyType,ConsumerPrxType>,
          class UnSubscriptionMakerType=detail::DefaultUnSubscriptionMaker<ProxyType,ConsumerPrxType> >
class PtrIceStormListener
{

public:

    PtrIceStormListener( const orcaice::Context  &context,
                      const std::string       &proxyStr )
        : data_(new DataType),
          context_(context),
          proxyString_(proxyStr),
          isSubscribed_(false)
        {
            // Register with the adapter
            consumer_ = new detail::PtrBufferedTimedConsumerI<ConsumerType,DataPtrType>;
            registerWithAdapter();
        }

    ~PtrIceStormListener()
        {
            shutdown();
            // do not delete consumer_, it's deletedwhen the smart pointers fall out of scope.
        }

    orcaice::PtrBuffer<DataPtrType> &buffer() { return consumer_->buffer_; }

    // Returns zero on success
    int connect()
    {
        // clean up first
        if ( shutdown() != 0 ) return -1;

        // then resubscribe
        try {
            Ice::ObjectPtr objPtr = consumer_;
            callbackPrx_ = orcaice::createConsumerInterface<ConsumerPrxType>( context_,
                                                                              objPtr );

            detail::subscribeListener<ProxyType,
                ConsumerType,
                ConsumerPrxType,
                SubscriptionMakerType>( context_, proxyString_, consumer_, callbackPrx_, proxy_ );
            isSubscribed_ = true;
            return 0;
        }
        catch ( ... ) {
            return -1;
        }
    };

    double msSinceReceipt() const { return consumer_->msSinceReceipt(); }
    void   resetTimer() { consumer_->resetTimer(); }
    std::string interfaceName() const { return proxyString_; };

private:

    DataPtrType                                                  data_;
    detail::PtrBufferedTimedConsumerI<ConsumerType,DataPtrType> *consumer_;

    orcaice::Context  context_;
    std::string       proxyString_;

    void registerWithAdapter()
        {
            Ice::ObjectPrx obj = context_.adapter()->addWithUUID( consumer_ );
            Ice::ObjectPrx prx = context_.adapter()->createDirectProxy( obj->ice_getIdentity() );
            callbackPrx_ = ConsumerPrxType::uncheckedCast( prx );
        }

    // 0=success
    int shutdown()
        {
            // std::cout<<"TRACE(icestormlistener.h): start of shutdown(): isSubscribed_: " << isSubscribed_ << std::endl;
            // unsubscribe
            if ( isSubscribed_ )
            {
                try {
                    detail::unSubscribeListener<ProxyType,
                        ConsumerType,
                        ConsumerPrxType,
                        UnSubscriptionMakerType>( context_, proxyString_, consumer_, callbackPrx_, proxy_ );

                    // remove consumer from the list of active servants
                    context_.adapter()->remove( callbackPrx_->ice_getIdentity() );

                    isSubscribed_ = false;
                    // std::cout<<"TRACE(icestormlistener.h): unsubscription successful." << std::endl;
                } catch ( ... ) {
                    return -1;
                }
            }
            return 0;
        }

    // Proxy to remote object
    ProxyType      proxy_;

    // Smart pointer to consumer_.
    // We need to hold onto this to keep it alive.
    ConsumerPrxType callbackPrx_;
    bool            isSubscribed_;
};


} // namespace

#endif
