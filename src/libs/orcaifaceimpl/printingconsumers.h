/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PRINTING_CONSUMERS_H
#define ORCAIFACEIMPL_PRINTING_CONSUMERS_H

#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/rangescanner2d.h>
#include <orca/localise3d.h>
#include <orca/localise2d.h>
#include <orca/odometry3d.h>
#include <orca/odometry2d.h>
#include <orca/drivebicycle.h>
#include <orca/ogmap.h>
#include <orca/gps.h>

namespace orcaifaceimpl
{

//
// An attempt to minimise template kung-fu in component code.
//

typedef PrintingConsumerImpl<orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       PrintingRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<PrintingRangeScanner2dConsumerImpl> PrintingRangeScanner2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Localise2dPrx,
                        orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              PrintingLocalise2dConsumerImpl;
typedef IceUtil::Handle<PrintingLocalise2dConsumerImpl>     PrintingLocalise2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Localise3dPrx,
                        orca::Localise3dConsumer,
                        orca::Localise3dConsumerPrx,
                        orca::Localise3dData>              PrintingLocalise3dConsumerImpl;
typedef IceUtil::Handle<PrintingLocalise3dConsumerImpl>     PrintingLocalise3dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              PrintingOdometry2dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry2dConsumerImpl>     PrintingOdometry2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Odometry3dPrx,
                        orca::Odometry3dConsumer,
                        orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              PrintingOdometry3dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry3dConsumerImpl>     PrintingOdometry3dConsumerImplPtr;


typedef PrintingConsumerImpl<orca::DriveBicyclePrx,
                        orca::DriveBicycleConsumer,
                        orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            PrintingDriveBicycleConsumerImpl;
typedef IceUtil::Handle<PrintingDriveBicycleConsumerImpl>   PrintingDriveBicycleConsumerImplPtr;

typedef PrintingConsumerImpl<orca::OgMapPrx,
                        orca::OgMapConsumer,
                        orca::OgMapConsumerPrx,
                        orca::OgMapData>                   PrintingOgMapConsumerImpl;
typedef IceUtil::Handle<PrintingOgMapConsumerImpl>          PrintingOgMapConsumerImplPtr;

typedef PrintingConsumerImpl<orca::GpsPrx,
                        orca::GpsConsumer,
                        orca::GpsConsumerPrx,
                        orca::GpsData>                     PrintingGpsConsumerImpl;
typedef IceUtil::Handle<PrintingGpsConsumerImpl>            PrintingGpsConsumerImplPtr;

} // namespace

#endif
