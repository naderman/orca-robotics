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

#include <orcaifaceimpl/proxiedconsumer.h>
#include <orca/rangescanner2d.h>
#include <orca/localise2d.h>
#include <orca/odometry2d.h>
#include <orca/drivebicycle.h>
#include <orca/ogmap.h>
#include <orca/gps.h>

namespace orcaifaceimpl
{

//
// An attempt to minimise template kung-fu in component code.
//

typedef ProxiedConsumer<orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       ProxiedRangeScanner2dConsumer;
typedef IceUtil::Handle<ProxiedRangeScanner2dConsumer>     ProxiedRangeScanner2dConsumerPtr;

typedef ProxiedConsumer<orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              ProxiedLocalise2dConsumer;
typedef IceUtil::Handle<ProxiedLocalise2dConsumer>         ProxiedLocalise2dConsumerPtr;

typedef ProxiedConsumer<orca::Odometry3dConsumer,
                        orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              ProxiedOdometry3dConsumer;
typedef IceUtil::Handle<ProxiedOdometry3dConsumer>         ProxiedOdometry3dConsumerPtr;

typedef ProxiedConsumer<orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              ProxiedOdometry2dConsumer;
typedef IceUtil::Handle<ProxiedOdometry2dConsumer>         ProxiedOdometry2dConsumerPtr;

typedef ProxiedConsumer<orca::DriveBicycleConsumer,
                        orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            ProxiedDriveBicycleConsumer;
typedef IceUtil::Handle<ProxiedDriveBicycleConsumer>       ProxiedDriveBicycleConsumerPtr;

typedef ProxiedConsumer<orca::OgMapConsumer,
                        orca::OgMapConsumerPrx,
                        orca::OgMapData>                   ProxiedOgMapConsumer;
typedef IceUtil::Handle<ProxiedOgMapConsumer>              ProxiedOgMapConsumerPtr;

typedef ProxiedConsumer<orca::GpsConsumer,
                        orca::GpsConsumerPrx,
                        orca::GpsData>                     ProxiedGpsConsumer;
typedef IceUtil::Handle<ProxiedGpsConsumer>                ProxiedGpsConsumerPtr;


} // namespace

#endif
