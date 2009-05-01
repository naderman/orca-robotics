/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_BINARYSWITCH_H
#define ORCAIFACEIMPL_BINARYSWITCH_H

#include <orcaifaceimpl/binaryswitchImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/binaryswitch.h>

namespace orcaifaceimpl {

typedef StoringConsumerImpl<orca::BinarySwitch, orca::BinarySwitchPrx,
                    orca::BinarySwitchConsumer, orca::BinarySwitchConsumerPrx,
                    orca::BinarySwitchData> StoringBinarySwitchConsumerImpl;
typedef IceUtil::Handle<StoringBinarySwitchConsumerImpl> StoringBinarySwitchConsumerImplPtr;

typedef BufferedConsumerImpl<orca::BinarySwitch, orca::BinarySwitchPrx,
                        orca::BinarySwitchConsumer, orca::BinarySwitchConsumerPrx,
                        orca::BinarySwitchData> BufferedBinarySwitchConsumerImpl;
typedef IceUtil::Handle<BufferedBinarySwitchConsumerImpl> BufferedBinarySwitchConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::BinarySwitch, orca::BinarySwitchPrx,
                        orca::BinarySwitchConsumer,
                        orca::BinarySwitchConsumerPrx,
                        orca::BinarySwitchData> NotifyingBinarySwitchConsumerImpl;
typedef IceUtil::Handle<NotifyingBinarySwitchConsumerImpl> NotifyingBinarySwitchConsumerImplPtr;

typedef PrintingConsumerImpl<orca::BinarySwitch, orca::BinarySwitchPrx,
                        orca::BinarySwitchConsumer, orca::BinarySwitchConsumerPrx,
                        orca::BinarySwitchData> PrintingBinarySwitchConsumerImpl;
typedef IceUtil::Handle<PrintingBinarySwitchConsumerImpl> PrintingBinarySwitchConsumerImplPtr;

}

#endif
