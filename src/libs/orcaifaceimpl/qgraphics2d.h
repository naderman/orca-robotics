/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_QGRAPHICS2D_H
#define ORCAIFACEIMPL_QGRAPHICS2D_H

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/qgraphics2d.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::QGraphics2d, orca::QGraphics2dConsumer,
                    orca::QGraphics2dData> QGraphics2dImpl;
typedef IceUtil::Handle<QGraphics2dImpl> QGraphics2dImplPtr;

typedef StoringConsumerImpl<orca::QGraphics2d, orca::QGraphics2dConsumer,
                    orca::QGraphics2dData> StoringQGraphics2dConsumerImpl;
typedef IceUtil::Handle<StoringQGraphics2dConsumerImpl> StoringQGraphics2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::QGraphics2d, orca::QGraphics2dConsumer,
                        orca::QGraphics2dData> BufferedQGraphics2dConsumerImpl;
typedef IceUtil::Handle<BufferedQGraphics2dConsumerImpl> BufferedQGraphics2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::QGraphics2d, orca::QGraphics2dConsumer,
                        orca::QGraphics2dData> NotifyingQGraphics2dConsumerImpl;
typedef IceUtil::Handle<NotifyingQGraphics2dConsumerImpl> NotifyingQGraphics2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::QGraphics2d, orca::QGraphics2dConsumer,
                        orca::QGraphics2dData> PrintingQGraphics2dConsumerImpl;
typedef IceUtil::Handle<PrintingQGraphics2dConsumerImpl> PrintingQGraphics2dConsumerImplPtr;

}

#endif
