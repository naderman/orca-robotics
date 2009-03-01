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
#include "defaultautologgerfactory.h"
#include "autologgers.h"

using namespace std;
using namespace orcalogfactory;

DefaultAutoLoggerFactory::DefaultAutoLoggerFactory()
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
    addSupportedType("Camera");
    addSupportedType("Image");
}

orcalog::AutoLogger* 
DefaultAutoLoggerFactory::create( const std::string &interfaceType )
{
    std::auto_ptr<orcalog::AutoLogger> logger;
    if (interfaceType == "Cpu")
    {
        logger.reset( new CpuAutoLogger );
    }
    else if (interfaceType == "DriveBicycle")
    {
        logger.reset( new DriveBicycleAutoLogger );
    }
    else if (interfaceType == "Imu")
    {
        logger.reset( new ImuAutoLogger );
    }
    else if (interfaceType == "LaserScanner2d")
    {
        logger.reset( new LaserScanner2dAutoLogger );
    }
    else if (interfaceType == "Localise2d")
    {
        logger.reset( new Localise2dAutoLogger );
    }
    else if (interfaceType == "Localise3d")
    {
        logger.reset( new Localise3dAutoLogger );
    }
    else if (interfaceType == "Odometry2d")
    {
        logger.reset( new Odometry2dAutoLogger );
    }
    else if (interfaceType == "Odometry3d")
    {
        logger.reset( new Odometry3dAutoLogger );
    }
    else if (interfaceType == "PolarFeature2d")
    {
        logger.reset( new PolarFeature2dAutoLogger );
    }
    else if (interfaceType == "Power")
    {
        logger.reset( new PowerAutoLogger );
    }
    else if (interfaceType == "Wifi")
    {
        logger.reset( new WifiAutoLogger );
    }
    else if (interfaceType == "Gps")
    {
        logger.reset( new GpsAutoLogger );
    }
    else if (interfaceType == "Camera")
    {
        logger.reset( new CameraAutoLogger );
    }
    else if (interfaceType == "Image")
    {
        logger.reset( new ImageAutoLogger );
    }
    else
    {
        return NULL;
    }

    return logger.release();
}

orcalog::AutoLoggerFactory* createAutoLoggerFactory()
{
    return new DefaultAutoLoggerFactory;
}
