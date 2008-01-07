/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_LISTENER_DETAIL_H
#define ORCAGUI_ICESTORM_LISTENER_DETAIL_H

#include <orcaice/ptrbuffer.h>

namespace orcaqguielementutil
{

// non-member utility stuff
namespace detail {

// A little bit of magic to get around the fact that different (un)subscription
// functions may have different names
template< class ProxyType, class ConsumerPrxType >
class DefaultSubscriptionMaker 
{
public:
    DefaultSubscriptionMaker( ProxyType proxy, ConsumerPrxType callbackPrx )
        { proxy->subscribe( callbackPrx ); }
};

template< class ProxyType, class ConsumerPrxType >
class DefaultUnSubscriptionMaker 
{
public:
    DefaultUnSubscriptionMaker( ProxyType proxy, ConsumerPrxType callbackPrx )
        { proxy->unsubscribe( callbackPrx ); }
};

//
// ================ Definition =====================
//

// Subscribe/unsubscribe IceStormListeners
template< class ProxyType,
            class ConsumerType,
            class ConsumerPrxType,
            class SubscriptionMakerType >
void
subscribeListener( orcaice::Context      &context,
                    const std::string     &proxyString,
                    ConsumerType          *consumer,
                    ConsumerPrxType       &callbackPrx,
                    ProxyType             &proxy);

template< class ProxyType,
            class ConsumerType,
            class ConsumerPrxType,
            class UnSubscriptionMakerType >
void
unSubscribeListener( orcaice::Context      &context,
                        const std::string     &proxyString,
                        ConsumerType          *consumer,
                        ConsumerPrxType       &callbackPrx,
                        ProxyType             &proxy);

//
// ================ Implementation =====================
//
            
template< class ProxyType,
            class ConsumerType,
            class ConsumerPrxType,
            class SubscriptionMakerType >
void
subscribeListener( orcaice::Context      &context,
                    const std::string     &proxyString,
                    ConsumerType          *consumer,
                    ConsumerPrxType       &callbackPrx,
                    ProxyType             &proxy )
{
    try {
        // Connect to remote interface
        orcaice::connectToInterfaceWithString( context, proxy, proxyString );

        // Ask the remote object to subscribe us to the topic.
        SubscriptionMakerType s( proxy, callbackPrx );
    }
    // Ignore all exceptions, and try again next time.
    catch ( Ice::ConnectionRefusedException &e ) {
        std::cout<<"TRACE(PtrIceStormListener::subscribeListener): Caught exception: " << e << std::endl;
        throw;
    }
    catch ( Ice::Exception &e ) {
        std::cout<<"TRACE(PtrIceStormListener::subscribeListener): Caught ice exception: " << e << std::endl;
        throw;
    }
    catch ( std::exception &e ) {
        std::cout<<"TRACE(PtrIceStormListener::subscribeListener): Caught std exception: " << e.what() << std::endl;
        throw;
    }
    catch ( ... ) {
        std::cout<<"TRACE(PtrIceStormListener::subscribeListener): Caught unknown exception." << std::endl;
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
                        ConsumerPrxType       &callbackPrx,
                        ProxyType             &proxy)
{
    try {
        // Connect to remote object
        orcaice::connectToInterfaceWithString( context, proxy, proxyString );

        // Ask the remote object to unsubscribe us from the topic.
        UnSubscriptionMakerType s( proxy, callbackPrx );
    }
    // Ignore all exceptions, and try again next time.
    catch ( Ice::ConnectionRefusedException &e ) {
        std::cout<<"TRACE(PtrIceStormListener::unSubscribeListener): Caught exception: " << e << std::endl;
        throw;
    }
    catch ( Ice::Exception &e ) {
        std::cout<<"TRACE(PtrIceStormListener::unSubscribeListener): Caught ice exception: " << e << std::endl;
        throw;
    }
    catch ( std::exception &e ) {
        std::cout<<"TRACE(PtrIceStormListener::unSubscribeListener): Caught std exception: " << e.what() << std::endl;
        throw;
    }
    catch ( ... ) {
        std::cout<<"TRACE(PtrIceStormListener::unSubscribeListener): Caught unknown exception." << std::endl;
        throw;
    }
}

//////////////////////////////////////////////////////////////////////

/*
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData().
 *
 */
template<class ConsumerType, class ObjectType>
class BufferedTimedConsumerI : public ConsumerType
{
public:

    BufferedTimedConsumerI( int depth=1,
                            hydroiceutil::BufferType bufferType = hydroiceutil::BufferTypeCircular )
        : buffer_( depth, bufferType ) {};

    void setData( const ObjectType& data, const Ice::Current& )
        {
            buffer_.push( data );
            handleData( data );
        }

    double msSinceReceipt() { return timer_.elapsedMs(); }
    void resetTimer() { return timer_.restart(); }

    // buffer_ is public so that guielements can access it directly
    hydroiceutil::Buffer<ObjectType> buffer_;

protected:

    void handleData( const ObjectType & obj )
        {
            timer_.restart();
        }

    hydroiceutil::Timer timer_;
};
/*
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData().
 *
 */
template<class ConsumerType, class ObjectType>
class PtrBufferedTimedConsumerI : public ConsumerType
{
public:

    PtrBufferedTimedConsumerI( int depth=1, hydroiceutil::BufferType bufferType = hydroiceutil::BufferTypeCircular )
        : buffer_( depth, bufferType ) {};

    void setData( const ObjectType& data, const Ice::Current& )
        {
            buffer_.push( data );
            handleData( data );
        }

    double msSinceReceipt() { return timer_.elapsedMs(); }
    void resetTimer() { return timer_.restart(); }

    // buffer_ is public so that guielements can access it directly
    orcaice::PtrBuffer<ObjectType> buffer_;

protected:

    void handleData( const ObjectType & obj )
        {
            timer_.restart();
        }

    hydroiceutil::Timer timer_;
};


} // namespace
} // namespace

#endif
