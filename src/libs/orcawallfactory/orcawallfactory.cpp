/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "orcawallfactory.h"

// all plug-ins
#include "laserscanner2dsim.h"
#include "localise2dsim.h"
#include "odometry2dsim.h"

using namespace orcawallfactory;

OrcaWallFactory::OrcaWallFactory()
{
//     addSupportedType("BinarySwitch");
//     addSupportedType("Camera");
//     addSupportedType("Cpu");
//     addSupportedType("DriveBicycle");
//     addSupportedType("Gps");
//     addSupportedType("Home");
    addSupportedType("LaserScanner2d");
//     addSupportedType("Wifi");
    addSupportedType("Localise2d");
//     addSupportedType("Localise3d");
//     addSupportedType("PolarFeature2d");
    addSupportedType("Odometry2d");
//     addSupportedType("Odometry3d");
//     addSupportedType("Power");
//     addSupportedType("PathFollower2d");
//     addSupportedType("Status");
//     addSupportedType("Tracer");
}

orcawall::InterfaceSim* 
OrcaWallFactory::create( const std::string& interfaceType, const std::string& tag, 
        const orcaice::Context& context )
{
    orcawall::InterfaceSim* sim = 0;

//     if ( interfaceType == "BinarySwitch" ) {
//         sim = new BinarySwitchSim( tag, context );
//     }
//     else if ( interfaceType == "Camera" ) {
//         sim = new CameraSim( tag, context );
//     }
//     else if ( interfaceType == "Cpu" ) {
//         sim = new CpuSim( tag, context );
//     }
//     else if ( interfaceType == "DriveBicycle" ) {
//     sim = new DriveBicycleSim( tag, context );
//     }
//     else if ( interfaceType == "Gps" ) {
//         sim = new GpsSim( tag, context );
//     }
//     else if ( interfaceType == "Home" ) {
//         sim = new HomeSim( tag, context );
//     }
//     else 
    if ( interfaceType == "LaserScanner2d" ) {
        sim = new LaserScanner2dSim( tag, context );
    }
//     else if ( interfaceType == "Wifi" ) {
//         sim = new WifiSim( tag, context );
//     }
    else if ( interfaceType == "Localise2d" ) {
        sim = new Localise2dSim( tag, context );
    }
//     else if ( interfaceType == "Localise3d" ) {
//         sim = new Localise3dSim( tag, context );
//     }
    else if ( interfaceType == "Odometry2d" ) {
        sim = new Odometry2dSim( tag, context );
    }
//     else if ( interfaceType == "Odometry3d" ) {
//         sim = new Odometry3dSim( tag, context );
//     }
//     else if ( interfaceType == "Power" ) {
//         sim = new PowerSim( tag, context );
//     }
//     else if ( interfaceType == "PathFollower2d" ) {
//         sim = new PathFollower2dSim( tag, context );
//     }
//     else if ( interfaceType == "Status" ) {
//         sim = new StatusSim( tag, context );
//     }
//     else if ( interfaceType == "Tracer" ) {
//         sim = new TracerSim( tag, context );
//     }

    return sim;
}

orcawall::Factory* createFactory()
{
    return new OrcaWallFactory;
}
