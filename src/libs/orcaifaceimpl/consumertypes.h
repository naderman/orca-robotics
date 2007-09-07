/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_CONSUMER_TYPES_H
#define ORCAIFACEIMPL_CONSUMER_TYEPS_H

#include <orcaifaceimpl/proxiedconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
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

//
// Proxied consumers
//

typedef ProxiedConsumerImpl<orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       ProxiedRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<ProxiedRangeScanner2dConsumerImpl> ProxiedRangeScanner2dConsumerImplPtr;

typedef ProxiedConsumerImpl<orca::Localise2dPrx,
                        orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              ProxiedLocalise2dConsumerImpl;
typedef IceUtil::Handle<ProxiedLocalise2dConsumerImpl>     ProxiedLocalise2dConsumerImplPtr;

typedef ProxiedConsumerImpl<orca::Localise3dPrx,
                        orca::Localise3dConsumer,
                        orca::Localise3dConsumerPrx,
                        orca::Localise3dData>              ProxiedLocalise3dConsumerImpl;
typedef IceUtil::Handle<ProxiedLocalise3dConsumerImpl>     ProxiedLocalise3dConsumerImplPtr;

typedef ProxiedConsumerImpl<orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              ProxiedOdometry2dConsumerImpl;
typedef IceUtil::Handle<ProxiedOdometry2dConsumerImpl>     ProxiedOdometry2dConsumerImplPtr;

typedef ProxiedConsumerImpl<orca::Odometry3dPrx,
                        orca::Odometry3dConsumer,
                        orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              ProxiedOdometry3dConsumerImpl;
typedef IceUtil::Handle<ProxiedOdometry3dConsumerImpl>     ProxiedOdometry3dConsumerImplPtr;


typedef ProxiedConsumerImpl<orca::DriveBicyclePrx,
                        orca::DriveBicycleConsumer,
                        orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            ProxiedDriveBicycleConsumerImpl;
typedef IceUtil::Handle<ProxiedDriveBicycleConsumerImpl>   ProxiedDriveBicycleConsumerImplPtr;

typedef ProxiedConsumerImpl<orca::OgMapPrx,
                        orca::OgMapConsumer,
                        orca::OgMapConsumerPrx,
                        orca::OgMapData>                   ProxiedOgMapConsumerImpl;
typedef IceUtil::Handle<ProxiedOgMapConsumerImpl>          ProxiedOgMapConsumerImplPtr;

typedef ProxiedConsumerImpl<orca::GpsPrx,
                        orca::GpsConsumer,
                        orca::GpsConsumerPrx,
                        orca::GpsData>                     ProxiedGpsConsumerImpl;
typedef IceUtil::Handle<ProxiedGpsConsumerImpl>            ProxiedGpsConsumerImplPtr;

//
// Buffered consumers
//
typedef BufferedConsumerImpl<orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              BufferedOdometry2dConsumerImpl;
typedef IceUtil::Handle<BufferedOdometry2dConsumerImpl>    BufferedOdometry2dConsumerImplPtr;


//
// Printing consumers
//
typedef PrintingConsumerImpl<orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              PrintingOdometry2dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry2dConsumerImpl>    PrintingOdometry2dConsumerImplPtr;

} // namespace

#endif
