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
                        orca::RangeScanner2dDataPtr>       proxiedRangeScanner2dConsumer;
typedef IceUtil::Handle<proxiedRangeScanner2dConsumer>     proxiedRangeScanner2dConsumerPtr;

typedef ProxiedConsumer<orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              proxiedLocalise2dConsumer;
typedef IceUtil::Handle<proxiedLocalise2dConsumer>         proxiedLocalise2dConsumerPtr;

typedef ProxiedConsumer<orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              proxiedOdometry2dConsumer;
typedef IceUtil::Handle<proxiedOdometry2dConsumer>         proxiedOdometry2dConsumerPtr;

typedef ProxiedConsumer<orca::DriveBicycleConsumer,
                        orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            proxiedDriveBicycleConsumer;
typedef IceUtil::Handle<proxiedDriveBicycleConsumer>       proxiedDriveBicycleConsumerPtr;

typedef ProxiedConsumer<orca::OgMapConsumer,
                        orca::OgMapConsumerPrx,
                        orca::OgMapData>                   proxiedOgMapConsumer;
typedef IceUtil::Handle<proxiedOgMapConsumer>              proxiedOgMapConsumerPtr;

typedef ProxiedConsumer<orca::GpsConsumer,
                        orca::GpsConsumerPrx,
                        orca::GpsData>                     proxiedGpsConsumer;
typedef IceUtil::Handle<proxiedGpsConsumer>                proxiedGpsConsumerPtr;


} // namespace

#endif
