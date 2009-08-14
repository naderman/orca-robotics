/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_LASERSCANNER2D_H
#define ORCAIFACEIMPL_LASERSCANNER2D_H

#include <orca/laserscanner2d.h>
#include <orcaifaceutil/laserscanner2d.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::LaserScanner2d, orca::RangeScanner2dConsumer,
                    orca::RangeScanner2dDataPtr, orca::RangeScanner2dDescription> LaserScanner2dImpl;
typedef IceUtil::Handle<LaserScanner2dImpl> LaserScanner2dImplPtr;

typedef ConsumerImpl<orca::LaserScanner2d, orca::RangeScanner2dConsumer,
                    orca::RangeScanner2dDataPtr> LaserScanner2dConsumerImpl;
typedef IceUtil::Handle<LaserScanner2dConsumerImpl> LaserScanner2dConsumerImplPtr;

typedef StoringConsumerImpl<orca::LaserScanner2d, orca::RangeScanner2dConsumer,
                    orca::RangeScanner2dDataPtr> StoringLaserScanner2dConsumerImpl;
typedef IceUtil::Handle<StoringLaserScanner2dConsumerImpl> StoringLaserScanner2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::LaserScanner2d, orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dDataPtr> BufferedLaserScanner2dConsumerImpl;
typedef IceUtil::Handle<BufferedLaserScanner2dConsumerImpl> BufferedLaserScanner2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::LaserScanner2d, orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dDataPtr> NotifyingLaserScanner2dConsumerImpl;
typedef IceUtil::Handle<NotifyingLaserScanner2dConsumerImpl> NotifyingLaserScanner2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::LaserScanner2d, orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dDataPtr> PrintingLaserScanner2dConsumerImpl;
typedef IceUtil::Handle<PrintingLaserScanner2dConsumerImpl> PrintingLaserScanner2dConsumerImplPtr;

}

#endif
