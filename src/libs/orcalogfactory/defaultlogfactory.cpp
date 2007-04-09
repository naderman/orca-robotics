/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

// #include <iostream>

#include <orcalog/logmaster.h>

// all plug-ins
#include "cameralogger.h"
#include "cpulogger.h"
#include "gpslogger.h"
#include "imulogger.h"
#include "laserscanner2dlogger.h"
#include "localise2dlogger.h"
#include "localise3dlogger.h"
#include "polarfeature2dlogger.h"
#include "odometry2dlogger.h"
#include "odometry3dlogger.h"
#include "powerlogger.h"
#include "wifilogger.h"

#include "defaultlogfactory.h"

using namespace std;
using namespace orcalogfactory;


DefaultLogFactory::DefaultLogFactory()
{
    addSupportedType("Camera");
    addSupportedType("Cpu");
    addSupportedType("Gps");
    addSupportedType("Imu");
    addSupportedType("LaserScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("Odometry2d");
    addSupportedType("Odometry3d");
    addSupportedType("PolarFeature2d");
    addSupportedType("Power");
    addSupportedType("Wifi");
}

orcalog::Logger* 
DefaultLogFactory::create( const std::string      &interfaceType, 
                           const std::string      &typeSuffix,
                           const std::string      &format,
                           orcalog::LogMaster     *master,
                           const std::string      &filenamePrefix,
                           const orcaice::Context &context )
{
    orcalog::Logger* logger = 0;
     
    if (interfaceType == "Camera")
    {
        logger = new CameraLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Cpu")
    {
        logger = new CpuLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Gps")
    {
        logger = new GpsLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Imu")
    {
        logger = new ImuLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "LaserScanner2d")
    {
        logger = new LaserScanner2dLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Localise2d")
    {
        logger = new Localise2dLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Localise3d")
    {
        logger = new Localise3dLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Odometry2d")
    {
        logger = new Odometry2dLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Odometry3d")
    {
        logger = new Odometry3dLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "PolarFeature2d")
    {
        logger = new PolarFeature2dLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Power")
    {
        logger = new PowerLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    else if (interfaceType == "Wifi")
    {
        logger = new WifiLogger( master, typeSuffix, format, filenamePrefix, context );
    }
    
    return logger;
}

orcalog::LogFactory* createLogFactory()
{
    return new DefaultLogFactory;
}
