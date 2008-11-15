/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_RANGESCANNER2D_H
#define ORCAIFACEIMPL_RANGESCANNER2D_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/rangescanner2d.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                    orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                    orca::RangeScanner2dDataPtr, orca::RangeScanner2dDescription> RangeScanner2dImpl;
typedef IceUtil::Handle<RangeScanner2dImpl> RangeScanner2dImplPtr;

typedef StoringConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                    orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                    orca::RangeScanner2dDataPtr> StoringRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<StoringRangeScanner2dConsumerImpl> StoringRangeScanner2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr> BufferedRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<BufferedRangeScanner2dConsumerImpl> BufferedRangeScanner2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr> NotifyingRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<NotifyingRangeScanner2dConsumerImpl> NotifyingRangeScanner2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr> PrintingRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<PrintingRangeScanner2dConsumerImpl> PrintingRangeScanner2dConsumerImplPtr;

}

#endif
