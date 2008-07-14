/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ICESTORM_ELEMENT_3D_H
#define ORCAGUI_ICESTORM_ELEMENT_3D_H

#include <orcaqguielementutil/icestormelement.h>
#include <orcaqgui3d/view.h>

namespace orcaqgui3d {

    template<class PainterType, class DataType, class ProxyType, class ConsumerType, class ConsumerPrxType>
    class
    IceStormElement3d : public orcaqguielementutil::IceStormElement<PainterType,
                                                                    DataType,
                                                                    ProxyType,
                                                                    ConsumerType,
                                                                    ConsumerPrxType>
    {
    public:

        //! timeoutMs is how long we wait before assuming a problem and trying to reconnect.
        //! (timoutMs = -1 means we never timeout)
        IceStormElement3d( const orcaice::Context  &context,
                           const std::string       &proxyString,
                           PainterType             &painter,
                           const double            timeoutMs=5000.0 )
            : orcaqguielementutilIceStormElement<PainterType,
                                                 DataType,
                                                 ProxyType,
                                                 ConsumerType,
                                                 ConsumerPrxType>(
                context,proxyString,painter,timeoutMs)
            {}

        //! Derived classes may override this if they have to paint other stuff than just interface data (e.g. user input)
        virtual void paint( const View &view )
            { IceStormElement<PainterType,
                  DataType,
                  ProxyType,
                  ConsumerType,
                  ConsumerPrxType>::painter_.paint( view ); }
    };

}

#endif
