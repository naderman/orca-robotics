/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_ELEMENT_3D_H
#define ORCAGUI_ICESTORM_ELEMENT_3D_H

#include <orcaqguielementutil/icestormelement.h>
#include <orcaqgui3d/guielement3d.h>

namespace orcaqgui3d {

    template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
    class
    IceStormElement3d : public GuiElement3d,
                        public orcaqguielementutil::IceStormElement<DataType,
                                                                    ProxyType,
                                                                    ConsumerType,
                                                                    ConsumerPrxType>
    {
    public:

        //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
        //! (timoutMs = -1 means we never timeout)
        IceStormElement3d( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                           const orcaice::Context                     &context,
                           PainterType                                &painter,
                           const double                                timeoutMs = 5000.0 )
            : GuiElement3d(guiElementInfo),
              orcaqguielementutil::IceStormElement<DataType,
                                                   ProxyType,
                                                   ConsumerType,
                                                   ConsumerPrxType>(
              context,guiElementInfo.uniqueId.toStdString(),timeoutMs),
              painter_(painter)
            {}

        // From GuiElement3d
        void update();
        
    protected:
        
        //! Object to handle painting to our data to screen
        PainterType                     &painter_;
};
    
template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
void IceStormElement3d<PainterType,DataType,ProxyType,ConsumerType,ConsumerPrxType>::update()
{ 
    if (orcaqguielementutil::IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::shouldClearPainter_)
    {
        painter_.clear();
        orcaqguielementutil::IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::shouldClearPainter_ = false;
    }
    
    if (orcaqguielementutil::IceStormElement<DataType,
                                             ProxyType,
                                             ConsumerType,
                                             ConsumerPrxType>::updateFromStore() )
    {               
        // transfer data into painter
        painter_.setData( orcaqguielementutil::IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>::data_ );
    }
}

}

#endif
