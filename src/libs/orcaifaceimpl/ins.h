/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_INS_H
#define ORCAIFACEIMPL_INS_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/ins.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Ins, orca::InsPrx,
                    orca::InsConsumer, orca::InsConsumerPrx,
                    orca::InsData, orca::InsDescription> InsImpl;
typedef IceUtil::Handle<InsImpl> InsImplPtr;

typedef StoringConsumerImpl<orca::Ins, orca::InsPrx,
                    orca::InsConsumer, orca::InsConsumerPrx,
                    orca::InsData> StoringInsConsumerImpl;
typedef IceUtil::Handle<StoringInsConsumerImpl> StoringInsConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Ins, orca::InsPrx,
                        orca::InsConsumer, orca::InsConsumerPrx,
                        orca::InsData> BufferedInsConsumerImpl;
typedef IceUtil::Handle<BufferedInsConsumerImpl> BufferedInsConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Ins, orca::InsPrx,
                        orca::InsConsumer,
                        orca::InsConsumerPrx,
                        orca::InsData> NotifyingInsConsumerImpl;
typedef IceUtil::Handle<NotifyingInsConsumerImpl> NotifyingInsConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Ins, orca::InsPrx,
                        orca::InsConsumer, orca::InsConsumerPrx,
                        orca::InsData> PrintingInsConsumerImpl;
typedef IceUtil::Handle<PrintingInsConsumerImpl> PrintingInsConsumerImplPtr;

}

#endif
