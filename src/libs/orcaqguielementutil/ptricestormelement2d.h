/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_PTRICESTORM_ELEMENT_2D_H
#define ORCAGUI_PTRICESTORM_ELEMENT_2D_H

#include <orcaqguielementutil/ptricestormelement.h>

namespace orcaqguielementutil {

    template<class PainterType, class DataType, class DataPtrType, class ProxyType, class ConsumerType, class ConsumerPrxType>
    class
    PtrIceStormElement2d : public hydroqguielementutil::GuiElement2d,
                           public PtrIceStormElement<PainterType,
                                                     DataType,
                                                     DataPtrType,
                                                     ProxyType,
                                                     ConsumerType,
                                                     ConsumerPrxType>
    {
    public:

        //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
        //! (timoutMs = -1 means we never timeout)
        PtrIceStormElement2d( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       PainterType             &painter,
                       const double            timeoutMs=5000.0 )
        : PtrIceStormElement<PainterType,DataType,DataPtrType,ProxyType,ConsumerType,ConsumerPrxType>(
            context,proxyString,painter,timeoutMs)
            {}
        

        //! Derived classes may override this if they have to paint other stuff than just interface data (e.g. user input)
        virtual void paint( QPainter *p, int z )
            { PtrIceStormElement<PainterType,
                  DataType,
                  DataPtrType,
                  ProxyType,
                  ConsumerType,
                  ConsumerPrxType>::painter_.paint( p, z ); }

        bool paintThisLayer( int z ) const
            { return PtrIceStormElement<PainterType,
                  DataType,
                  DataPtrType,
                  ProxyType,
                  ConsumerType,
                  ConsumerPrxType>::painter_.paintThisLayer( z ); }

        //! From GuiElement2d
        void update() 
            { PtrIceStormElement<PainterType,
                  DataType,
                  DataPtrType,
                  ProxyType,
                  ConsumerType,
                  ConsumerPrxType>::updateFromBuffer(); }
    };

}

#endif
