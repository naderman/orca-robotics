/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_IMU_H
#define ORCAIFACEIMPL_IMU_H

#include <orca/imu.h>
#include <orcaifaceutil/imu.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Imu, orca::ImuConsumer,
                    orca::ImuData, orca::ImuDescription> ImuImpl;
typedef IceUtil::Handle<ImuImpl> ImuImplPtr;

typedef StoringConsumerImpl<orca::Imu, orca::ImuConsumer,
                    orca::ImuData> StoringImuConsumerImpl;
typedef IceUtil::Handle<StoringImuConsumerImpl> StoringImuConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Imu, orca::ImuConsumer,
                        orca::ImuData> BufferedImuConsumerImpl;
typedef IceUtil::Handle<BufferedImuConsumerImpl> BufferedImuConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Imu, orca::ImuConsumer,
                        orca::ImuData> NotifyingImuConsumerImpl;
typedef IceUtil::Handle<NotifyingImuConsumerImpl> NotifyingImuConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Imu, orca::ImuConsumer,
                        orca::ImuData> PrintingImuConsumerImpl;
typedef IceUtil::Handle<PrintingImuConsumerImpl> PrintingImuConsumerImplPtr;

}

#endif
