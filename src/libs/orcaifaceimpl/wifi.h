/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_WIFI_H
#define ORCAIFACEIMPL_WIFI_H

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/wifi.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::Wifi, orca::WifiPrx,
                    orca::WifiConsumer, orca::WifiConsumerPrx,
                    orca::WifiData> WifiImpl;
typedef IceUtil::Handle<WifiImpl> WifiImplPtr;

typedef StoringConsumerImpl<orca::Wifi, orca::WifiPrx,
                    orca::WifiConsumer, orca::WifiConsumerPrx,
                    orca::WifiData> StoringWifiConsumerImpl;
typedef IceUtil::Handle<StoringWifiConsumerImpl> StoringWifiConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Wifi, orca::WifiPrx,
                        orca::WifiConsumer, orca::WifiConsumerPrx,
                        orca::WifiData> BufferedWifiConsumerImpl;
typedef IceUtil::Handle<BufferedWifiConsumerImpl> BufferedWifiConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Wifi, orca::WifiPrx,
                        orca::WifiConsumer,
                        orca::WifiConsumerPrx,
                        orca::WifiData> NotifyingWifiConsumerImpl;
typedef IceUtil::Handle<NotifyingWifiConsumerImpl> NotifyingWifiConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Wifi, orca::WifiPrx,
                        orca::WifiConsumer, orca::WifiConsumerPrx,
                        orca::WifiData> PrintingWifiConsumerImpl;
typedef IceUtil::Handle<PrintingWifiConsumerImpl> PrintingWifiConsumerImplPtr;

}

#endif
