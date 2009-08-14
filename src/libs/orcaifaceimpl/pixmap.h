/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_PIXMAP_H
#define ORCAIFACEIMPL_PIXMAP_H

#include <orca/pixmap.h>
#include <orcaifaceutil/pixmap.h>

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::PixMap, orca::PixMapConsumer,
                    orca::PixMapData> PixMapImpl;
typedef IceUtil::Handle<PixMapImpl> PixMapImplPtr;

typedef ConsumerImpl<orca::PixMap, orca::PixMapConsumer,
                    orca::PixMapData> PixMapConsumerImpl;
typedef IceUtil::Handle<PixMapConsumerImpl> PixMapConsumerImplPtr;

typedef StoringConsumerImpl<orca::PixMap, orca::PixMapConsumer,
                    orca::PixMapData> StoringPixMapConsumerImpl;
typedef IceUtil::Handle<StoringPixMapConsumerImpl> StoringPixMapConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PixMap, orca::PixMapConsumer,
                        orca::PixMapData> BufferedPixMapConsumerImpl;
typedef IceUtil::Handle<BufferedPixMapConsumerImpl> BufferedPixMapConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PixMap, orca::PixMapConsumer,
                        orca::PixMapData> NotifyingPixMapConsumerImpl;
typedef IceUtil::Handle<NotifyingPixMapConsumerImpl> NotifyingPixMapConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PixMap, orca::PixMapConsumer,
                        orca::PixMapData> PrintingPixMapConsumerImpl;
typedef IceUtil::Handle<PrintingPixMapConsumerImpl> PrintingPixMapConsumerImplPtr;

}

#endif
