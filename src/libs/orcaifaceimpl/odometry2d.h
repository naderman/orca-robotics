/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_ODOMETRY2D_H
#define ORCAIFACEIMPL_ODOMETRY2D_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/odometry2d.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Odometry2d, orca::Odometry2dPrx,
                    orca::Odometry2dConsumer, orca::Odometry2dConsumerPrx,
                    orca::Odometry2dData, orca::VehicleDescription> Odometry2dImpl;
typedef IceUtil::Handle<Odometry2dImpl> Odometry2dImplPtr;

typedef StoringConsumerImpl<orca::Odometry2d, orca::Odometry2dPrx,
                    orca::Odometry2dConsumer, orca::Odometry2dConsumerPrx,
                    orca::Odometry2dData> StoringOdometry2dConsumerImpl;
typedef IceUtil::Handle<StoringOdometry2dConsumerImpl> StoringOdometry2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Odometry2d, orca::Odometry2dPrx,
                        orca::Odometry2dConsumer, orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData> BufferedOdometry2dConsumerImpl;
typedef IceUtil::Handle<BufferedOdometry2dConsumerImpl> BufferedOdometry2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Odometry2d, orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData> NotifyingOdometry2dConsumerImpl;
typedef IceUtil::Handle<NotifyingOdometry2dConsumerImpl> NotifyingOdometry2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Odometry2d, orca::Odometry2dPrx,
                        orca::Odometry2dConsumer, orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData> PrintingOdometry2dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry2dConsumerImpl> PrintingOdometry2dConsumerImplPtr;

}

#endif
