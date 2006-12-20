/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006 Alexei Makarenko
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
#include "homeprobe.h"
#include "laserscanner2dprobe.h"
#include "powerprobe.h"
#include "statusprobe.h"
#include "tracerprobe.h"

using namespace orcaprobefactory;

OrcaProbeFactory::OrcaProbeFactory()
{
    addSupportedType("::orca::BinarySwitch");
    addSupportedType("::orca::Camera");
    addSupportedType("::orca::Cpu");
//     addSupportedType("::orca::Gps");
    addSupportedType("::orca::Home");
    addSupportedType("::orca::LaserScanner2d");
//     addSupportedType("::orca::Localise2d");
//     addSupportedType("::orca::PolarFeature2d");
//     addSupportedType("::orca::Position2d");
//     addSupportedType("::orca::Position3d");
    addSupportedType("::orca::Power");
    addSupportedType("::orca::Status");
    addSupportedType("::orca::Tracer");
}

orcaprobe::InterfaceProbe* 
OrcaProbeFactory::create( const std::string           & interfaceType, 
                          const orca::FQInterfaceName & name, 
                          orcaprobe::DisplayDriver    & display, 
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
    else if ( interfaceType == "::orca::Home" ) {
        probe = new HomeProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::LaserScanner2d" ) {
        probe = new LaserScanner2dProbe( name, display, context );
    }
    else if ( interfaceType == "::orca::Power" ) {
        probe = new PowerProbe( name, display, context );
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
