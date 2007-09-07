/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_BUFFERED_CONSUMERS_H
#define ORCAIFACEIMPL_BUFFERED_CONSUMERS_H

#include <orcaifaceimpl/bufferedconsumerImpl.h>

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

typedef BufferedConsumerImpl<orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       BufferedRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<BufferedRangeScanner2dConsumerImpl> BufferedRangeScanner2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Localise2dPrx,
                        orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              BufferedLocalise2dConsumerImpl;
typedef IceUtil::Handle<BufferedLocalise2dConsumerImpl>    BufferedLocalise2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Localise3dPrx,
                        orca::Localise3dConsumer,
                        orca::Localise3dConsumerPrx,
                        orca::Localise3dData>              BufferedLocalise3dConsumerImpl;
typedef IceUtil::Handle<BufferedLocalise3dConsumerImpl>    BufferedLocalise3dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Odometry2dPrx,
                        orca::Odometry2dConsumer,
                        orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              BufferedOdometry2dConsumerImpl;
typedef IceUtil::Handle<BufferedOdometry2dConsumerImpl>    BufferedOdometry2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Odometry3dPrx,
                        orca::Odometry3dConsumer,
                        orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              BufferedOdometry3dConsumerImpl;
typedef IceUtil::Handle<BufferedOdometry3dConsumerImpl>    BufferedOdometry3dConsumerImplPtr;


typedef BufferedConsumerImpl<orca::DriveBicyclePrx,
                        orca::DriveBicycleConsumer,
                        orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            BufferedDriveBicycleConsumerImpl;
typedef IceUtil::Handle<BufferedDriveBicycleConsumerImpl>  BufferedDriveBicycleConsumerImplPtr;

typedef BufferedConsumerImpl<orca::OgMapPrx,
                        orca::OgMapConsumer,
                        orca::OgMapConsumerPrx,
                        orca::OgMapData>                   BufferedOgMapConsumerImpl;
typedef IceUtil::Handle<BufferedOgMapConsumerImpl>         BufferedOgMapConsumerImplPtr;

typedef BufferedConsumerImpl<orca::GpsPrx,
                        orca::GpsConsumer,
                        orca::GpsConsumerPrx,
                        orca::GpsData>                     BufferedGpsConsumerImpl;
typedef IceUtil::Handle<BufferedGpsConsumerImpl>           BufferedGpsConsumerImplPtr;


} // namespace

#endif
