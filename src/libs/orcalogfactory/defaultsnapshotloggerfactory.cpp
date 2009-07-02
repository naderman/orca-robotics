/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

// #include <iostream>
#include "defaultsnapshotloggerfactory.h"
#include "snapshotloggers.h"

using namespace std;
using namespace orcalogfactory;

DefaultSnapshotLoggerFactory::DefaultSnapshotLoggerFactory()
{
    addSupportedType("Cpu");
    addSupportedType("DriveBicycle");
    addSupportedType("Imu");
    addSupportedType("LaserScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("Odometry2d");
    addSupportedType("Odometry3d");
    addSupportedType("PolarFeature2d");
    addSupportedType("Power");
    addSupportedType("Wifi");
    addSupportedType("Gps");
}

orcalog::SnapshotLogger* 
DefaultSnapshotLoggerFactory::create( const std::string &interfaceType, const orcaice::Context& context )
{
    std::auto_ptr<orcalog::SnapshotLogger> logger;
    if (interfaceType == "Cpu")
    {
        logger.reset( new CpuSnapshotLogger( context ) );
    }
    else if (interfaceType == "DriveBicycle")
    {
        logger.reset( new DriveBicycleSnapshotLogger( context ) );
    }
    else if (interfaceType == "Imu")
    {
        logger.reset( new ImuSnapshotLogger( context ) );
    }
    else if (interfaceType == "LaserScanner2d")
    {
        logger.reset( new LaserScanner2dSnapshotLogger( context ) );
    }
    else if (interfaceType == "Localise2d")
    {
        logger.reset( new Localise2dSnapshotLogger( context ) );
    }
    else if (interfaceType == "Localise3d")
    {
        logger.reset( new Localise3dSnapshotLogger( context ) );
    }
    else if (interfaceType == "Odometry2d")
    {
        logger.reset( new Odometry2dSnapshotLogger( context ) );
    }
    else if (interfaceType == "Odometry3d")
    {
        logger.reset( new Odometry3dSnapshotLogger( context ) );
    }
    else if (interfaceType == "PolarFeature2d")
    {
        logger.reset( new PolarFeature2dSnapshotLogger( context ) );
    }
    else if (interfaceType == "Power")
    {
        logger.reset( new PowerSnapshotLogger( context ) );
    }
    else if (interfaceType == "Wifi")
    {
        logger.reset( new WifiSnapshotLogger( context ) );
    }
    else if (interfaceType == "Gps")
    {
        logger.reset( new GpsSnapshotLogger( context ) );
    }
    else
    {
        return NULL;
    }

    return logger.release();
}

orcalog::SnapshotLoggerFactory* createSnapshotLoggerFactory()
{
    return new DefaultSnapshotLoggerFactory;
}
