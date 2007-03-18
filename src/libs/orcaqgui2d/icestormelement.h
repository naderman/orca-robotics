/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_ELEMENT_H
#define ORCAGUI_ICESTORM_ELEMENT_H

#include <assert.h>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>
#include <orcaqgui/icestormlistener.h>

#include <orcaqgui2d/guielement2d.h>

namespace orcaqgui2d {

/*!
  @brief Base class for gui elements that receive data from IceStorm.

  Connects to an interface, passes info to painter

  @author Alex Brooks
*/
template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
class IceStormElement : public orcaqgui2d::GuiElement2d
{
  
public:

    //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
    //! (timoutMs = -1 means we never timeout)
    IceStormElement( const orcaice::Context  &context,
                     const std::string       &proxyString,
                     PainterType             &painter,
                     const double            timeoutMs=5000.0 )
        : context_(context),
          listener_(context,proxyString),
          painter_(painter),
          timeoutMs_(timeoutMs),
          isConnected_(false)
        {};

    //! Can do special stuff on connection by inheriting and overloading this
    virtual void actionOnConnection()=0;

    //! Here we pop data from the consumer buffer, and give it to the painter.
    virtual void update();

    //! Derived classes may override this if they have to paint other stuff than just interface data (e.g. user input)
    virtual void paint( QPainter *p, int z )
        { painter_.paint( p, z ); }

    bool paintThisLayer( int z ) const
        { return painter_.paintThisLayer( z ); }

protected:

    orcaice::Context              context_;
    
    DataType                       data_;
    
    bool needToUpdate();
    
    //! Object to handle network communication, including subscription process
    orcaqgui::IceStormListener<DataType,
                     ProxyType,
                     ConsumerType,
                     ConsumerPrxType> listener_;

    //! Object to handle painting to our data to screen
    PainterType                     &painter_;
    
    double                           timeoutMs_;
    bool                             isConnected_;
};

template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
bool 
IceStormElement<PainterType,DataType,ProxyType,ConsumerType,ConsumerPrxType>::needToUpdate()
{
    if ( !listener_.buffer().isEmpty() )
    {
        // An object has arrived in our buffer.  We need to update.
        return true;
    }

    if ( !isConnected_ )
    {
        if ( listener_.connect() == 0 )
        {
            isConnected_ = true;
            actionOnConnection();
        }
        return false;
    }

    // The buffer is empty.  How long since we last received something?
    if ( timeoutMs_ != -1 && listener_.msSinceReceipt() >= timeoutMs_ )
    {
        std::cout<<"TRACE(icestormelement.h): Haven't received anything from " 
                 << listener_.interfaceName() << " for " << listener_.msSinceReceipt() << "ms" << std::endl;
        std::cout<<"TRACE(icestormelement.h): Timing out..." << std::endl;

        painter_.clear();
        listener_.resetTimer();
        if ( listener_.connect() == 0 )
            actionOnConnection();
    }
    return false;
}

template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
void 
IceStormElement<PainterType,DataType,ProxyType,ConsumerType,ConsumerPrxType>::update()
{
    if ( !needToUpdate() ) {
        return;
    }
    
    assert( !listener_.buffer().isEmpty() );

    // get data from the buffer
    listener_.buffer().getAndPop( data_ );

    // transfer data into painter
    painter_.setData( data_ );
}

} // namespace

#endif
