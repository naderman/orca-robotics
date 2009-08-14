/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_ODOMETRY3D_H
#define ORCAIFACEIMPL_ODOMETRY3D_H

#include <orca/odometry3d.h>
#include <orcaifaceutil/odometry3d.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Odometry3d, orca::Odometry3dConsumer,
                    orca::Odometry3dData, orca::VehicleDescription> Odometry3dImpl;
typedef IceUtil::Handle<Odometry3dImpl> Odometry3dImplPtr;

typedef StoringConsumerImpl<orca::Odometry3d, orca::Odometry3dConsumer,
                    orca::Odometry3dData> StoringOdometry3dConsumerImpl;
typedef IceUtil::Handle<StoringOdometry3dConsumerImpl> StoringOdometry3dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Odometry3d, orca::Odometry3dConsumer,
                        orca::Odometry3dData> BufferedOdometry3dConsumerImpl;
typedef IceUtil::Handle<BufferedOdometry3dConsumerImpl> BufferedOdometry3dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Odometry3d, orca::Odometry3dConsumer,
                        orca::Odometry3dData> NotifyingOdometry3dConsumerImpl;
typedef IceUtil::Handle<NotifyingOdometry3dConsumerImpl> NotifyingOdometry3dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Odometry3d, orca::Odometry3dConsumer,
                        orca::Odometry3dData> PrintingOdometry3dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry3dConsumerImpl> PrintingOdometry3dConsumerImplPtr;

}

#endif
