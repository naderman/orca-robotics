/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
//    addSupportedType("Camera");
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

orcalog::AutoLogger* 
DefaultAutoLoggerFactory::create( const orcalog::LogWriterInfo &logWriterInfo )
{
//     if (interfaceType == "Camera")
//     {
//         return new CameraLogger( masterFileWriter, comment, format, filename, context );
//     }
    if (logWriterInfo.interfaceType == "Cpu")
    {
        return new CpuAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "DriveBicycle")
    {
        return new DriveBicycleAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Imu")
    {
        return new ImuAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "LaserScanner2d")
    {
        return new LaserScanner2dAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Localise2d")
    {
        return new Localise2dAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Localise3d")
    {
        return new Localise3dAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Odometry2d")
    {
        return new Odometry2dAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Odometry3d")
    {
        return new Odometry3dAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "PolarFeature2d")
    {
        return new PolarFeature2dAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Power")
    {
        return new PowerAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Wifi")
    {
        return new WifiAutoLogger( logWriterInfo );
    }
    else if (logWriterInfo.interfaceType == "Gps")
    {
        return new GpsAutoLogger( logWriterInfo );
    }
    else
    {
        return NULL;
    }
}

orcalog::AutoLoggerFactory* createAutoLoggerFactory()
{
    return new DefaultAutoLoggerFactory;
}
