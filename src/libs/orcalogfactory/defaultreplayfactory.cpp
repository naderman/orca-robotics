/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>  // just for basename()

// all plug-ins
#include "camerareplayer.h"
#include "gpsreplayer.h"
#include "laserscanner2dreplayer.h"
#include "position2dreplayer.h"
#include "position3dreplayer.h"
#include "powerreplayer.h"

#include "defaultreplayfactory.h"

using namespace std;
using namespace orcalogfactory;

DefaultReplayFactory::DefaultReplayFactory()
{
    addSupportedType("Camera");
//     addSupportedType("Cpu");
    addSupportedType("Gps");
    addSupportedType("LaserScanner2d");
//     addSupportedType("Localise2d");
//     addSupportedType("PolarFeature2d");
    addSupportedType("Position2d");
    addSupportedType("Position3d");
    addSupportedType("Power");
}

orcalog::Replayer* 
DefaultReplayFactory::create( 
    const std::string      &interfaceType, 
    const std::string      &format,
    const std::string      &filename,
    const orcaice::Context &context)
{
    orcalog::Replayer *replayer = 0;
    
    context.tracer()->debug( "Creating replayer: file="+filename+" type="+interfaceType+" fmt="+format, 5);

    if ( interfaceType == "Camera" )
    {
        replayer = new CameraReplayer( format, filename, context );
    }
    else if ( interfaceType == "Gps" )
    {
        replayer = new GpsReplayer( format, filename, context );
    }
    if ( interfaceType == "LaserScanner2d" )
    {
        replayer = new LaserScanner2dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Position2d" )
    {
        replayer = new Position2dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Position3d" )
    {
        replayer = new Position3dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Power" )
    {
        replayer = new PowerReplayer( format, filename, context );
    }
    
    return replayer;
}

orcalog::ReplayFactory* createReplayFactory()
{
    return new DefaultReplayFactory;
}

