/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_LISTENER_H
#define ORCAGUI_ICESTORM_LISTENER_H

#include <iostream>
#include <assert.h>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaice/bufferedtimedconsumerI.h>
#include <orcaice/connectutils.h>
#include <orcaice/context.h>
#include <orcaice/timer.h>

namespace orcaqgui
{

// non-member utility stuff
namespace detail {

    // A little bit of magic to get around the fact that different (un)subscription
    // functions may have different names
    template< class ProxyType, class ConsumerPrxType >
    class DefaultSubscriptionMaker {
    public:
        DefaultSubscriptionMaker( ProxyType proxy, ConsumerPrxType callbackPrx )
            { proxy->subscribe( callbackPrx ); }
    };
    template< class ProxyType, class ConsumerPrxType >
    class DefaultUnSubscriptionMaker {
    public:
        DefaultUnSubscriptionMaker( ProxyType proxy, ConsumerPrxType callbackPrx )
            { proxy->unsubscribe( callbackPrx ); }
    };

    // Subscribe/unsubscribe IceStormListeners

    template< class ProxyType,
              class ConsumerType,
              class ConsumerPrxType,
              class SubscriptionMakerType >
    void
    subscribeListener( orcaice::Context      &context,
                       const std::string     &proxyString,
                       ConsumerType          *consumer,
                       ConsumerPrxType       &callbackPrx );
    template< class ProxyType,
              class ConsumerType,
              class ConsumerPrxType,
              class UnSubscriptionMakerType >
    void
    unSubscribeListener( orcaice::Context      &context,
                         const std::string     &proxyString,
                         ConsumerType          *consumer,
                         ConsumerPrxType       &callbackPrx );
}

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
class IceStormListener
{

public:

    IceStormListener( const orcaice::Context  &context,
                      const std::string       &proxyStr )
        : data_(new DataType),
          context_(context),
          proxyString_(proxyStr),
          isSubscribed_(false)
        {
            // Register with the adapter
            consumer_ = new orcaice::BufferedTimedConsumerI<ConsumerType,DataPtrType>;
            registerWithAdapter();
        }

    ~IceStormListener()
        {
            shutdown();
            // delete timer_;
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
            Ice::ObjectPrx obj = context_.adapter()->addWithUUID( consumer_ );
            Ice::ObjectPrx prx = context_.adapter()->createDirectProxy( obj->ice_getIdentity() );
            callbackPrx_ = ConsumerPrxType::uncheckedCast( prx );
            
            detail::subscribeListener<ProxyType,
                ConsumerType,
                ConsumerPrxType,
                SubscriptionMakerType>( context_, proxyString_, consumer_, callbackPrx_ );
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

    DataPtrType                                                data_;
    orcaice::BufferedTimedConsumerI<ConsumerType,DataPtrType> *consumer_;

    orcaice::Context  context_;
    std::string       proxyString_;

    orcaice::Timer timer_;

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
                        UnSubscriptionMakerType>( context_, proxyString_, consumer_, callbackPrx_ );

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



namespace detail {

    template< class ProxyType,
              class ConsumerType,
              class ConsumerPrxType,
              class SubscriptionMakerType >
    void
    subscribeListener( orcaice::Context      &context,
                       const std::string     &proxyString,
                       ConsumerType          *consumer,
                       ConsumerPrxType       &callbackPrx )
    {
        try {
            // Connect to remote interface
            ProxyType proxy;
            orcaice::connectToInterfaceWithString( context, proxy, proxyString );

            // Ask the remote object to subscribe us to the topic.
            SubscriptionMakerType s( proxy, callbackPrx );
        }
        // Ignore all exceptions, and try again next time.
        catch ( Ice::ConnectionRefusedException &e ) {
            std::cout<<"TRACE(IceStormListener::subscribeListener): Caught exception: " << e << std::endl;
            throw;
        }
        catch ( Ice::Exception &e ) {
            std::cout<<"TRACE(IceStormListener::subscribeListener): Caught ice exception: " << e << std::endl;
            throw;
        }
        catch ( std::exception &e ) {
            std::cout<<"TRACE(IceStormListener::subscribeListener): Caught std exception: " << e.what() << std::endl;
            throw;
        }
        catch ( ... ) {
            std::cout<<"TRACE(IceStormListener::subscribeListener): Caught unknown exception." << std::endl;
            throw;
        }
    }
    template< class ProxyType,
              class ConsumerType,
              class ConsumerPrxType,
              class UnSubscriptionMakerType >
    void
    unSubscribeListener( orcaice::Context      &context,
                         const std::string     &proxyString,
                         ConsumerType          *consumer,
                         ConsumerPrxType       &callbackPrx )
    {
        try {
            // Connect to remote object
            ProxyType proxy;
            orcaice::connectToInterfaceWithString( context, proxy, proxyString );

            // Ask the remote object to unsubscribe us from the topic.
            UnSubscriptionMakerType s( proxy, callbackPrx );
        }
        // Ignore all exceptions, and try again next time.
        catch ( Ice::ConnectionRefusedException &e ) {
            std::cout<<"TRACE(IceStormListener::unSubscribeListener): Caught exception: " << e << std::endl;
            throw;
        }
        catch ( Ice::Exception &e ) {
            std::cout<<"TRACE(IceStormListener::unSubscribeListener): Caught ice exception: " << e << std::endl;
            throw;
        }
        catch ( std::exception &e ) {
            std::cout<<"TRACE(IceStormListener::unSubscribeListener): Caught std exception: " << e.what() << std::endl;
            throw;
        }
        catch ( ... ) {
            std::cout<<"TRACE(IceStormListener::unSubscribeListener): Caught unknown exception." << std::endl;
            throw;
        }
    }
}

} // namespace

#endif
