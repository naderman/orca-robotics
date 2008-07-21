/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_PTRICESTORM_ELEMENT_3D_H
#define ORCAGUI_PTRICESTORM_ELEMENT_3D_H

#include <orcaqguielementutil/ptricestormelement.h>
#include <orcaqgui3d/view.h>
#include <QGLWidget>
#include <orcaqgui3d/guielement3d.h>

namespace orcaqgui3d {

    template<class PainterType, class DataType, class DataPtrType, class ProxyType, class ConsumerType, class ConsumerPrxType>
    class
    PtrIceStormElement3d : public GuiElement3d,
                           public orcaqguielementutil::PtrIceStormElement<PainterType,
                                                                          DataType,
                                                                          DataPtrType,
                                                                          ProxyType,
                                                                          ConsumerType,
                                                                          ConsumerPrxType>
    {
    public:

        //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
        //! (timoutMs = -1 means we never timeout)
        PtrIceStormElement3d( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       PainterType             &painter,
                       const double            timeoutMs=5000.0 )
        : orcaqguielementutil::PtrIceStormElement<PainterType,
                                                  DataType,
                                                  DataPtrType,
                                                  ProxyType,
                                                  ConsumerType,
                                                  ConsumerPrxType>(
            context,proxyString,painter,timeoutMs)
            {}
        
        // From GuiElement3d
        void update()
            { orcaqguielementutil::PtrIceStormElement<PainterType,
                  DataType,
                  DataPtrType,
                  ProxyType,
                  ConsumerType,
                  ConsumerPrxType>::updateFromBuffer(); }

    };

}

#endif
