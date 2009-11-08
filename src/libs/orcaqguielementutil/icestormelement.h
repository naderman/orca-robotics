/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_ELEMENT_H
#define ORCAGUI_ICESTORM_ELEMENT_H

#include <assert.h>
#include <orcaqguielementutil/icestormlistener.h>

namespace orcaqguielementutil {

/*!
  @brief Base class for gui elements that receive data from IceStorm.

  Connects to an interface

  @author Alex Brooks, Tobias Kaupp
*/
template<class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
class IceStormElement
{
  
public:

    //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
    //! (timoutMs = -1 means we never timeout)
    IceStormElement( const orcaice::Context                     &context,
                     const std::string                          &proxyString,
                     const double                                timeoutMs,
                     const double                                tryConnectIntervalSec = 2.5 )
        : context_(context),
          listener_(context,proxyString),
          tryConnectTimer_(IceUtil::Time::seconds(999)),
          tryConnectIntervalSec_(tryConnectIntervalSec),
          timeoutMs_(timeoutMs),
          isConnected_(false),
          shouldClearPainter_(false)
    {};

    virtual ~IceStormElement() {}

    //! Can do special stuff on connection by inheriting and overloading this
    virtual void iceStormConnectedEvent() {};

protected:

    //! Here we pop data from the consumer store
    bool updateFromStore();

    orcaice::Context context_;
    
    // alexm: this is a copy of the data in Store of the listener_
    // this duplicates data storage requirements
    DataType data_;
    
    bool needToUpdate();
    
    //! Object to handle network communication, including subscription process
    orcaqguielementutil::IceStormListener<DataType,
                                          ProxyType,
                                          ConsumerType,
                                          ConsumerPrxType> listener_;

    //! Controls the frequency with which we try to connect
    gbxiceutilacfr::Timer tryConnectTimer_;
    double tryConnectIntervalSec_;
    
    double timeoutMs_;
    bool isConnected_;
    bool shouldClearPainter_;
};

template<class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
bool 
IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::needToUpdate()
{
    if ( !isConnected_ )
    {
        if ( tryConnectTimer_.elapsedSec() < tryConnectIntervalSec_ )
            return false;
        tryConnectTimer_.restart();

        if ( listener_.connect() == 0 )
        {
            isConnected_ = true;
            assert( listener_.proxy() != 0 );
            iceStormConnectedEvent();
        }
        return false;
    }

    if ( listener_.store().isNewData() )
    {
        // An object has arrived in our buffer.  We need to update.
        return true;
    }

    // The store has nothing new.  How long since we last received something?
    if ( timeoutMs_ != -1 && listener_.msSinceReceipt() >= timeoutMs_ )
    {
        std::cout<<"TRACE(icestormelement.h): Haven't received anything from " 
                 << listener_.interfaceName() << " for " << listener_.msSinceReceipt() << "ms" << std::endl;
        std::cout<<"TRACE(icestormelement.h): Timing out..." << std::endl;

        shouldClearPainter_ = true;
        listener_.resetTimer();
        if ( listener_.connect() == 0 )
            iceStormConnectedEvent();
        else
            isConnected_ = false;
    }
    return false;
}

template<class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
bool 
IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::updateFromStore()
{
    if ( !needToUpdate() )
        return false;
    
    assert( listener_.store().isNewData() );

    // get data from the buffer
    listener_.store().get( data_ );
    
    return true;
}

} // namespace

#endif
