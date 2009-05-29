/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_GUIELEMENT_H
#define ORCAGUI_ICESTORM_GUIELEMENT_H

#include <orcaqguielementutil/icestormelement.h>
#include <hydroqguielementutil/guielement.h>

namespace orcaqguielementutil {


template<class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
class IceStormGuiElement : public hydroqguielementutil::GuiElement,
                           public IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>                                 
{
  
    public:
    
        IceStormGuiElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                            const orcaice::Context                     &context,
                            const std::string                          &proxyString,
                            const double                                timeoutMs,
                            const double                                tryConnectIntervalSec = 2.5 )
        : GuiElement(guiElementInfo),
          IceStormElement<DataType,ProxyType,ConsumerType,ConsumerPrxType>(context, proxyString, timeoutMs, tryConnectIntervalSec)
          
        {}
    
        virtual ~IceStormGuiElement() {};
        
        virtual bool isInGlobalCS() { return false; };
};

} // namespace

#endif
