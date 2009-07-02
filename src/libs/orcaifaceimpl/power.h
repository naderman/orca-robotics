/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_POWER_H
#define ORCAIFACEIMPL_POWER_H

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/power.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::Power, orca::PowerConsumer,
                    orca::PowerData> PowerImpl;
typedef IceUtil::Handle<PowerImpl> PowerImplPtr;

typedef StoringConsumerImpl<orca::Power, orca::PowerConsumer,
                    orca::PowerData> StoringPowerConsumerImpl;
typedef IceUtil::Handle<StoringPowerConsumerImpl> StoringPowerConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Power, orca::PowerConsumer,
                        orca::PowerData> BufferedPowerConsumerImpl;
typedef IceUtil::Handle<BufferedPowerConsumerImpl> BufferedPowerConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Power, orca::PowerConsumer,
                        orca::PowerData> NotifyingPowerConsumerImpl;
typedef IceUtil::Handle<NotifyingPowerConsumerImpl> NotifyingPowerConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Power, orca::PowerConsumer,
                        orca::PowerData> PrintingPowerConsumerImpl;
typedef IceUtil::Handle<PrintingPowerConsumerImpl> PrintingPowerConsumerImplPtr;

}

#endif
