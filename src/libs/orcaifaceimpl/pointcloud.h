/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_POINTCLOUD_H
#define ORCAIFACEIMPL_POINTCLOUD_H

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/pointcloud.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::PointCloud, orca::PointCloudPrx,
                    orca::PointCloudConsumer, orca::PointCloudConsumerPrx,
                    orca::PointCloudData> PointCloudImpl;
typedef IceUtil::Handle<PointCloudImpl> PointCloudImplPtr;

typedef StoringConsumerImpl<orca::PointCloud, orca::PointCloudPrx,
                    orca::PointCloudConsumer, orca::PointCloudConsumerPrx,
                    orca::PointCloudData> StoringPointCloudConsumerImpl;
typedef IceUtil::Handle<StoringPointCloudConsumerImpl> StoringPointCloudConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PointCloud, orca::PointCloudPrx,
                        orca::PointCloudConsumer, orca::PointCloudConsumerPrx,
                        orca::PointCloudData> BufferedPointCloudConsumerImpl;
typedef IceUtil::Handle<BufferedPointCloudConsumerImpl> BufferedPointCloudConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PointCloud, orca::PointCloudPrx,
                        orca::PointCloudConsumer,
                        orca::PointCloudConsumerPrx,
                        orca::PointCloudData> NotifyingPointCloudConsumerImpl;
typedef IceUtil::Handle<NotifyingPointCloudConsumerImpl> NotifyingPointCloudConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PointCloud, orca::PointCloudPrx,
                        orca::PointCloudConsumer, orca::PointCloudConsumerPrx,
                        orca::PointCloudData> PrintingPointCloudConsumerImpl;
typedef IceUtil::Handle<PrintingPointCloudConsumerImpl> PrintingPointCloudConsumerImplPtr;

}

#endif
