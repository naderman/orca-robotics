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
#include "laserscanner2dreplayer.h"
#include "localise2dreplayer.h"
#include "localise3dreplayer.h"
#include "powerreplayer.h"
#include "odometry2dreplayer.h"
#include "odometry3dreplayer.h"
#include "wifireplayer.h"


#include "defaultreplayfactory.h"

using namespace std;
using namespace orcalogfactory;

DefaultReplayFactory::DefaultReplayFactory()
{
    addSupportedType("Camera");
    addSupportedType("LaserScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("Odometry2d");
    addSupportedType("Odometry3d");
    addSupportedType("Power");
    addSupportedType("Wifi");
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
    else if ( interfaceType == "LaserScanner2d" )
    {
        replayer = new LaserScanner2dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Localise2d" )
    {
        replayer = new Localise2dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Localise3d" )
    {
        replayer = new Localise3dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Odometry2d" )
    {
        replayer = new Odometry2dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Odometry3d" )
    {
        replayer = new Odometry3dReplayer( format, filename, context );
    }
    else if ( interfaceType == "Power" )
    {
        replayer = new PowerReplayer( format, filename, context );
    }
    else if ( interfaceType == "Wifi" )
    {
        replayer = new WifiReplayer( format, filename, context );
    }
    
    return replayer;
}

orcalog::ReplayFactory* createReplayFactory()
{
    return new DefaultReplayFactory;
}

