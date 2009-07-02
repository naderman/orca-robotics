/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_LISTENER_H
#define ORCAGUI_ICESTORM_LISTENER_H

#include <assert.h>

#include <orcaice/icestormutils.h>
#include <orcaice/context.h>

#include <orcaqguielementutil/icestormlistenerdetail.h>

namespace orcaqguielementutil
{

/*!
    \brief Listens to IceStorm and stores the incoming data.
    
    Handles stuff like maintaining time since last receipt of data.
    
    \author Alex Brooks
*/
template< class DataType,
          class ProxyType,
          class ConsumerType,
          class ConsumerPrxType,
          class SubscriptionMakerType=detail::DefaultSubscriptionMaker<ProxyType,ConsumerPrxType>,
          class UnSubscriptionMakerType=detail::DefaultUnSubscriptionMaker<ConsumerPrxType> >
class IceStormListener
{

public:

    IceStormListener( const orcaice::Context  &context,
                      const std::string       &proxyStr )
        : context_(context),
          proxyString_(proxyStr),
          isSubscribed_(false)
        {
            // Register with the adapter
            consumer_ = new detail::StoredTimedConsumerI<ConsumerType,DataType>;
            registerWithAdapter();
        }

    ~IceStormListener()
        {
            shutdown();
            // do not delete consumer_, it's deleted when the smart pointers fall out of scope.
        }

    gbxiceutilacfr::Store<DataType> &store() { return consumer_->store_; }

    // Returns zero on success
    int connect()
    {
        // clean up first
        shutdown();

        // then resubscribe
        try {
            Ice::ObjectPtr objPtr = consumer_;
            callbackPrx_ = orcaice::createConsumerInterface<ConsumerPrxType>( context_,
                                                                              objPtr );

            topicPrx_ = detail::subscribeListener<ProxyType,
                ConsumerType,
                ConsumerPrxType,
                SubscriptionMakerType>( context_, proxyString_, consumer_, callbackPrx_, proxy_ );
            assert( proxy_ != 0 );
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
    
    const ProxyType& proxy() { return proxy_; };

private:

    detail::StoredTimedConsumerI<ConsumerType,DataType> *consumer_;

    orcaice::Context  context_;
    std::string       proxyString_;
    IceStorm::TopicPrx topicPrx_;

    void registerWithAdapter()
        {
            Ice::ObjectPrx obj = context_.adapter()->addWithUUID( consumer_ );
            Ice::ObjectPrx prx = context_.adapter()->createDirectProxy( obj->ice_getIdentity() );
            callbackPrx_ = ConsumerPrxType::uncheckedCast( prx );
        }

    void shutdown()
        {
            // std::cout<<"TRACE(icestormlistener.h): start of shutdown(): isSubscribed_: " << isSubscribed_ << std::endl;
            // unsubscribe
            if ( isSubscribed_ )
            {
                try {
                    detail::unSubscribeListener<ConsumerType,
                        ConsumerPrxType,
                        UnSubscriptionMakerType>( context_, topicPrx_, consumer_, callbackPrx_ );
                } catch ( ... ) {}

                try {
                    // remove consumer from the list of active servants
                    context_.adapter()->remove( callbackPrx_->ice_getIdentity() );
                } catch ( ... ) {}

                isSubscribed_ = false;
            }
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
