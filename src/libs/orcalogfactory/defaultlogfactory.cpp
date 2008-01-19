#if 0
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

#include <orcalog/masterfilewriter.h>

// all plug-insv
#include "cameralogger.h"
#include "cpulogger.h"
#include "drivebicyclelogger.h"
#include "imulogger.h"
#include "laserscanner2dlogger.h"
#include "localise2dlogger.h"
#include "localise3dlogger.h"
#include "polarfeature2dlogger.h"
#include "odometry2dlogger.h"
#include "odometry3dlogger.h"
#include "powerlogger.h"
#include "wifilogger.h"
#include "gpslogger.h"

#include "defaultlogfactory.h"

using namespace std;
using namespace orcalogfactory;


DefaultLogFactory::DefaultLogFactory()
{
    addSupportedType("Camera");
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

orcalog::Logger* 
DefaultLogFactory::create( const std::string         &interfaceType, 
                           const std::string         &comment,
                           const std::string         &format,
                           orcalog::MasterFileWriter &masterFileWriter,
                           const std::string         &filename,
                           const orcaice::Context    &context )
{
    orcalog::Logger* logger = 0;
     
    if (interfaceType == "Camera")
    {
        logger = new CameraLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Cpu")
    {
        logger = new CpuLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "DriveBicycle")
    {
        logger = new DriveBicycleLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Imu")
    {
        logger = new ImuLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "LaserScanner2d")
    {
        logger = new LaserScanner2dLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Localise2d")
    {
        logger = new Localise2dLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Localise3d")
    {
        logger = new Localise3dLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Odometry2d")
    {
        logger = new Odometry2dLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Odometry3d")
    {
        logger = new Odometry3dLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "PolarFeature2d")
    {
        logger = new PolarFeature2dLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Power")
    {
        logger = new PowerLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Wifi")
    {
        logger = new WifiLogger( masterFileWriter, comment, format, filename, context );
    }
    else if (interfaceType == "Gps")
    {
        logger = new GpsLogger( masterFileWriter, comment, format, filename, context );
    }
    
    return logger;
}

orcalog::LogFactory* createLogFactory()
{
    return new DefaultLogFactory;
}
#endif
