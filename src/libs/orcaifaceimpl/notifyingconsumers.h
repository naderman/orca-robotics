/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_NOTIFYING_CONSUMERS_H
#define ORCAIFACEIMPL_NOTIFYING_CONSUMERS_H

#include <orcaifaceimpl/notifyingconsumerImpl.h>

#include <orca/rangescanner2d.h>
#include <orca/localise3d.h>
#include <orca/localise2d.h>
#include <orca/odometry3d.h>
#include <orca/odometry2d.h>
#include <orca/drivebicycle.h>
#include <orca/ogmap.h>
#include <orca/gps.h>
#include <orca/polarfeature2d.h>

namespace orcaifaceimpl
{

//
// An attempt to minimise template kung-fu in component code.
//

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       NotifyingRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<NotifyingRangeScanner2dConsumerImpl> NotifyingRangeScanner2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::Localise2dPrx,
                        orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              NotifyingLocalise2dConsumerImpl;
typedef IceUtil::Handle<NotifyingLocalise2dConsumerImpl>     NotifyingLocalise2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::Localise3dPrx,
                        orca::Localise3dConsumer,
                        orca::Localise3dConsumerPrx,
                        orca::Localise3dData>              NotifyingLocalise3dConsumerImpl;
typedef IceUtil::Handle<NotifyingLocalise3dConsumerImpl>     NotifyingLocalise3dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              NotifyingOdometry2dConsumerImpl;
typedef IceUtil::Handle<NotifyingOdometry2dConsumerImpl>     NotifyingOdometry2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::Odometry3dPrx,
                        orca::Odometry3dConsumer,
                        orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              NotifyingOdometry3dConsumerImpl;
typedef IceUtil::Handle<NotifyingOdometry3dConsumerImpl>     NotifyingOdometry3dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::DriveBicyclePrx,
                        orca::DriveBicycleConsumer,
                        orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            NotifyingDriveBicycleConsumerImpl;
typedef IceUtil::Handle<NotifyingDriveBicycleConsumerImpl>   NotifyingDriveBicycleConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::OgMapPrx,
                        orca::OgMapConsumer,
                        orca::OgMapConsumerPrx,
                        orca::OgMapData>                   NotifyingOgMapConsumerImpl;
typedef IceUtil::Handle<NotifyingOgMapConsumerImpl>          NotifyingOgMapConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::GpsPrx,
                        orca::GpsConsumer,
                        orca::GpsConsumerPrx,
                        orca::GpsData>                     NotifyingGpsConsumerImpl;
typedef IceUtil::Handle<NotifyingGpsConsumerImpl>            NotifyingGpsConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef NotifyingConsumerImpl<orca::PolarFeature2dPrx,
                        orca::PolarFeature2dConsumer,
                        orca::PolarFeature2dConsumerPrx,
                        orca::PolarFeature2dData>              NotifyingPolarFeature2dConsumerImpl;
typedef IceUtil::Handle<NotifyingPolarFeature2dConsumerImpl>   NotifyingPolarFeature2dConsumerImplPtr;

} // namespace

#endif
