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
    addSupportedType("RangeScanner2d");
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
    addSupportedType("PathFollower2d");
}

orcalog::AutoLogger* 
DefaultAutoLoggerFactory::create( const std::string &interfaceType, const orcaice::Context& context )
{
    std::auto_ptr<orcalog::AutoLogger> logger;
    if (interfaceType == "Cpu")
    {
        logger.reset( new CpuAutoLogger( context ) );
    }
    else if (interfaceType == "DriveBicycle")
    {
        logger.reset( new DriveBicycleAutoLogger( context ) );
    }
    else if (interfaceType == "Imu")
    {
        logger.reset( new ImuAutoLogger( context ) );
    }
    else if (interfaceType == "LaserScanner2d")
    {
        logger.reset( new LaserScanner2dAutoLogger( context ) );
    }
    else if (interfaceType == "RangeScanner2d")
    {
        logger.reset( new RangeScanner2dAutoLogger( context ) );
    }
    else if (interfaceType == "Localise2d")
    {
        logger.reset( new Localise2dAutoLogger( context ) );
    }
    else if (interfaceType == "Localise3d")
    {
        logger.reset( new Localise3dAutoLogger( context ) );
    }
    else if (interfaceType == "Odometry2d")
    {
        logger.reset( new Odometry2dAutoLogger( context ) );
    }
    else if (interfaceType == "Odometry3d")
    {
        logger.reset( new Odometry3dAutoLogger( context ) );
    }
    else if (interfaceType == "PolarFeature2d")
    {
        logger.reset( new PolarFeature2dAutoLogger( context ) );
    }
    else if (interfaceType == "Power")
    {
        logger.reset( new PowerAutoLogger( context ) );
    }
    else if (interfaceType == "Wifi")
    {
        logger.reset( new WifiAutoLogger( context ) );
    }
    else if (interfaceType == "Gps")
    {
        logger.reset( new GpsAutoLogger( context ) );
    }
    else if (interfaceType == "Camera")
    {
        logger.reset( new CameraAutoLogger( context ) );
    }
    else if (interfaceType == "Image")
    {
        logger.reset( new ImageAutoLogger( context ) );
    }
    else if (interfaceType == "PathFollower2d")
    {
        logger.reset( new PathFollower2dAutoLogger( context ) );
    }
    else if (interfaceType == "MultiCamera")
    {
        logger.reset( new MultiCameraAutoLogger( context) );
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
