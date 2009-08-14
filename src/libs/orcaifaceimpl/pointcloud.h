/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_POINTCLOUD_H
#define ORCAIFACEIMPL_POINTCLOUD_H

#include <orca/pointcloud.h>
#include <orcaifaceutil/pointcloud.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::PointCloud, orca::PointCloudConsumer,
                    orca::PointCloudData, orca::PointCloudDescription> PointCloudImpl;
typedef IceUtil::Handle<PointCloudImpl> PointCloudImplPtr;

typedef StoringConsumerImpl<orca::PointCloud, orca::PointCloudConsumer,
                    orca::PointCloudData> StoringPointCloudConsumerImpl;
typedef IceUtil::Handle<StoringPointCloudConsumerImpl> StoringPointCloudConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PointCloud, orca::PointCloudConsumer,
                        orca::PointCloudData> BufferedPointCloudConsumerImpl;
typedef IceUtil::Handle<BufferedPointCloudConsumerImpl> BufferedPointCloudConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PointCloud, orca::PointCloudConsumer,
                        orca::PointCloudData> NotifyingPointCloudConsumerImpl;
typedef IceUtil::Handle<NotifyingPointCloudConsumerImpl> NotifyingPointCloudConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PointCloud, orca::PointCloudConsumer,
                        orca::PointCloudData> PrintingPointCloudConsumerImpl;
typedef IceUtil::Handle<PrintingPointCloudConsumerImpl> PrintingPointCloudConsumerImplPtr;

}

#endif
