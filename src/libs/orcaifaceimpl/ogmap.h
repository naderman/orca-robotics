/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_OGMAP_H
#define ORCAIFACEIMPL_OGMAP_H

#include <orca/ogmap.h>
#include <orcaifaceutil/ogmap.h>

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::OgMap, orca::OgMapConsumer,
                    orca::OgMapData> OgMapImpl;
typedef IceUtil::Handle<OgMapImpl> OgMapImplPtr;

typedef StoringConsumerImpl<orca::OgMap, orca::OgMapConsumer,
                    orca::OgMapData> StoringOgMapConsumerImpl;
typedef IceUtil::Handle<StoringOgMapConsumerImpl> StoringOgMapConsumerImplPtr;

typedef BufferedConsumerImpl<orca::OgMap, orca::OgMapConsumer,
                        orca::OgMapData> BufferedOgMapConsumerImpl;
typedef IceUtil::Handle<BufferedOgMapConsumerImpl> BufferedOgMapConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::OgMap, orca::OgMapConsumer,
                        orca::OgMapData> NotifyingOgMapConsumerImpl;
typedef IceUtil::Handle<NotifyingOgMapConsumerImpl> NotifyingOgMapConsumerImplPtr;

typedef PrintingConsumerImpl<orca::OgMap, orca::OgMapConsumer,
                        orca::OgMapData> PrintingOgMapConsumerImpl;
typedef IceUtil::Handle<PrintingOgMapConsumerImpl> PrintingOgMapConsumerImplPtr;

}

#endif
