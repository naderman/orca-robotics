/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_STORING_CONSUMERS_H
#define ORCAIFACEIMPL_STORING_CONSUMERS_H

#include <orcaifaceimpl/storingconsumerImpl.h>

#include <orca/rangescanner2d.h>
#include <orca/localise3d.h>
#include <orca/localise2d.h>
#include <orca/odometry3d.h>
#include <orca/odometry2d.h>
#include <orca/drivebicycle.h>
#include <orca/ogmap.h>
#include <orca/gps.h>
#include <orca/pathplanner2d.h>
#include <orca/properties.h>
#include <orca/ins.h>
#include <orca/pointcloud.h>

namespace orcaifaceimpl
{

//
// An attempt to minimise template kung-fu in component code.
//

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       StoringRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<StoringRangeScanner2dConsumerImpl> StoringRangeScanner2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Localise2d, orca::Localise2dPrx,
                        orca::Localise2dConsumer, orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              StoringLocalise2dConsumerImpl;
typedef IceUtil::Handle<StoringLocalise2dConsumerImpl>     StoringLocalise2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Localise3d, orca::Localise3dPrx,
                        orca::Localise3dConsumer, orca::Localise3dConsumerPrx,
                        orca::Localise3dData>              StoringLocalise3dConsumerImpl;
typedef IceUtil::Handle<StoringLocalise3dConsumerImpl>     StoringLocalise3dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Odometry2d, orca::Odometry2dPrx,
                        orca::Odometry2dConsumer, orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              StoringOdometry2dConsumerImpl;
typedef IceUtil::Handle<StoringOdometry2dConsumerImpl>     StoringOdometry2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Odometry3d, orca::Odometry3dPrx,
                        orca::Odometry3dConsumer, orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              StoringOdometry3dConsumerImpl;
typedef IceUtil::Handle<StoringOdometry3dConsumerImpl>     StoringOdometry3dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::DriveBicycle, orca::DriveBicyclePrx,
                        orca::DriveBicycleConsumer, orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            StoringDriveBicycleConsumerImpl;
typedef IceUtil::Handle<StoringDriveBicycleConsumerImpl>   StoringDriveBicycleConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::OgMap, orca::OgMapPrx,
                        orca::OgMapConsumer, orca::OgMapConsumerPrx,
                        orca::OgMapData>                   StoringOgMapConsumerImpl;
typedef IceUtil::Handle<StoringOgMapConsumerImpl>          StoringOgMapConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Gps, orca::GpsPrx,
                        orca::GpsConsumer, orca::GpsConsumerPrx,
                        orca::GpsData>                     StoringGpsConsumerImpl;
typedef IceUtil::Handle<StoringGpsConsumerImpl>            StoringGpsConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::PathPlanner2d, orca::PathPlanner2dPrx,
                        orca::PathPlanner2dConsumer, orca::PathPlanner2dConsumerPrx,
                        orca::PathPlanner2dData>           StoringPathPlanner2dConsumerImpl;
typedef IceUtil::Handle<StoringPathPlanner2dConsumerImpl>  StoringPathPlanner2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Properties, orca::PropertiesPrx,
                        orca::PropertiesConsumer, orca::PropertiesConsumerPrx,
                        orca::PropertiesData>           StoringPropertiesConsumerImpl;
typedef IceUtil::Handle<StoringPropertiesConsumerImpl>  StoringPropertiesConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::Ins, orca::InsPrx,
                        orca::InsConsumer, orca::InsConsumerPrx,
                        orca::InsData>                     StoringInsConsumerImpl;
typedef IceUtil::Handle<StoringInsConsumerImpl>            StoringInsConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef StoringConsumerImpl<orca::PointCloud, orca::PointCloudPrx,
                        orca::PointCloudConsumer, orca::PointCloudConsumerPrx,
                        orca::PointCloudData>                     StoringPointCloudConsumerImpl;
typedef IceUtil::Handle<StoringPointCloudConsumerImpl>            StoringPointCloudConsumerImplPtr;

} // namespace

#endif
