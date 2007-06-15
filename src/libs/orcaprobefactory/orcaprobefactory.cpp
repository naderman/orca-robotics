/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "orcaprobefactory.h"

// all plug-ins
#include "binaryswitchprobe.h"
#include "cameraprobe.h"
#include "cpuprobe.h"
#include "drivebicycleprobe.h"
#include "homeprobe.h"
#include "laserscanner2dprobe.h"
#include "localise2dprobe.h"
#include "localise3dprobe.h"
#include "odometry2dprobe.h"
#include "odometry3dprobe.h"
#include "gpsprobe.h"
#include "powerprobe.h"
#include "statusprobe.h"
#include "tracerprobe.h"
#include "pathfollower2dprobe.h"

using namespace orcaprobefactory;

OrcaProbeFactory::OrcaProbeFactory()
{
    addSupportedType("::orca::BinarySwitch");
    addSupportedType("::orca::Camera");
    addSupportedType("::orca::Cpu");
    addSupportedType("::orca::DriveBicycle");
    addSupportedType("::orca::Gps");
    addSupportedType("::orca::Home");
    addSupportedType("::orca::LaserScanner2d");
    addSupportedType("::orca::Localise2d");
    addSupportedType("::orca::Localise3d");
//     addSupportedType("::orca::PolarFeature2d");
    addSupportedType("::orca::Odometry2d");
    addSupportedType("::orca::Odometry3d");
    addSupportedType("::orca::Power");
    addSupportedType("::orca::PathFollower2d");
    addSupportedType("::orca::Status");
    addSupportedType("::orca::Tracer");
}

orcaprobe::InterfaceProbe* 
OrcaProbeFactory::create( const std::string           & interfaceType, 
                          const orca::FQInterfaceName & name, 
                          orcaprobe::IDisplay    & display, 
                          const orcaice::Context      & context )
{
    orcaprobe::InterfaceProbe* probe = 0;

    if ( interfaceType == "::orca::BinarySwitch" ) {
        probe = new BinarySwitchProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Camera" ) {
        probe = new CameraProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Cpu" ) {
        probe = new CpuProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::DriveBicycle" ) {
    probe = new DriveBicycleProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Gps" ) {
        probe = new GpsProbe( name, display, interfaceType, context );
    }
    else if ( interfaceType == "::orca::Home" ) {
        probe = new HomeProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::LaserScanner2d" ) {
        probe = new LaserScanner2dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Localise2d" ) {
        probe = new Localise2dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Localise3d" ) {
        probe = new Localise3dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Odometry2d" ) {
        probe = new Odometry2dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Odometry3d" ) {
        probe = new Odometry3dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Power" ) {
        probe = new PowerProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::PathFollower2d" ) {
        probe = new PathFollower2dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Status" ) {
        probe = new StatusProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Tracer" ) {
        probe = new TracerProbe( name, display, context );
    }

    return probe;
}

orcaprobe::Factory* createFactory()
{
    return new OrcaProbeFactory;
}
