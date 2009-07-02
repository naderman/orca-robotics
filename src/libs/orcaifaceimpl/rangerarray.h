/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_RANGERARRAY_H
#define ORCAIFACEIMPL_RANGERARRAY_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/rangerarray.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::RangerArray, orca::RangerArrayConsumer,
                    orca::RangerArrayDataPtr, orca::RangerArrayDescription> RangerArrayImpl;
typedef IceUtil::Handle<RangerArrayImpl> RangerArrayImplPtr;

typedef StoringConsumerImpl<orca::RangerArray, orca::RangerArrayConsumer,
                    orca::RangerArrayDataPtr> StoringRangerArrayConsumerImpl;
typedef IceUtil::Handle<StoringRangerArrayConsumerImpl> StoringRangerArrayConsumerImplPtr;

typedef BufferedConsumerImpl<orca::RangerArray, orca::RangerArrayConsumer,
                        orca::RangerArrayDataPtr> BufferedRangerArrayConsumerImpl;
typedef IceUtil::Handle<BufferedRangerArrayConsumerImpl> BufferedRangerArrayConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::RangerArray, orca::RangerArrayConsumer,
                        orca::RangerArrayDataPtr> NotifyingRangerArrayConsumerImpl;
typedef IceUtil::Handle<NotifyingRangerArrayConsumerImpl> NotifyingRangerArrayConsumerImplPtr;

typedef PrintingConsumerImpl<orca::RangerArray, orca::RangerArrayConsumer,
                        orca::RangerArrayDataPtr> PrintingRangerArrayConsumerImpl;
typedef IceUtil::Handle<PrintingRangerArrayConsumerImpl> PrintingRangerArrayConsumerImplPtr;

}

#endif
