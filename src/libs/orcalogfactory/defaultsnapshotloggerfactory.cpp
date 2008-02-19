/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
DefaultSnapshotLoggerFactory::create( const std::string &interfaceType )
{
    std::auto_ptr<orcalog::SnapshotLogger> logger;
    if (interfaceType == "Cpu")
    {
        logger.reset( new CpuSnapshotLogger );
    }
    else if (interfaceType == "DriveBicycle")
    {
        logger.reset( new DriveBicycleSnapshotLogger );
    }
    else if (interfaceType == "Imu")
    {
        logger.reset( new ImuSnapshotLogger );
    }
    else if (interfaceType == "LaserScanner2d")
    {
        logger.reset( new LaserScanner2dSnapshotLogger );
    }
    else if (interfaceType == "Localise2d")
    {
        logger.reset( new Localise2dSnapshotLogger );
    }
    else if (interfaceType == "Localise3d")
    {
        logger.reset( new Localise3dSnapshotLogger );
    }
    else if (interfaceType == "Odometry2d")
    {
        logger.reset( new Odometry2dSnapshotLogger );
    }
    else if (interfaceType == "Odometry3d")
    {
        logger.reset( new Odometry3dSnapshotLogger );
    }
    else if (interfaceType == "PolarFeature2d")
    {
        logger.reset( new PolarFeature2dSnapshotLogger );
    }
    else if (interfaceType == "Power")
    {
        logger.reset( new PowerSnapshotLogger );
    }
    else if (interfaceType == "Wifi")
    {
        logger.reset( new WifiSnapshotLogger );
    }
    else if (interfaceType == "Gps")
    {
        logger.reset( new GpsSnapshotLogger );
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
