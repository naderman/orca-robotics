/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_STATUS_H
#define ORCAIFACEIMPL_STATUS_H

#include <orca/status.h>
#include <orcaifaceutil/status.h>

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::Status, orca::StatusConsumer,
                    orca::StatusData> StatusImpl;
typedef IceUtil::Handle<StatusImpl> StatusImplPtr;

typedef StoringConsumerImpl<orca::Status, orca::StatusConsumer,
                    orca::StatusData> StoringStatusConsumerImpl;
typedef IceUtil::Handle<StoringStatusConsumerImpl> StoringStatusConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Status, orca::StatusConsumer,
                        orca::StatusData> BufferedStatusConsumerImpl;
typedef IceUtil::Handle<BufferedStatusConsumerImpl> BufferedStatusConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Status, orca::StatusConsumer,
                        orca::StatusData> NotifyingStatusConsumerImpl;
typedef IceUtil::Handle<NotifyingStatusConsumerImpl> NotifyingStatusConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Status, orca::StatusConsumer,
                        orca::StatusData> PrintingStatusConsumerImpl;
typedef IceUtil::Handle<PrintingStatusConsumerImpl> PrintingStatusConsumerImplPtr;

}

#endif
