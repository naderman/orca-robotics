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
#include "replayers.h"
#include "defaultreplayerfactory.h"

using namespace std;
using namespace orcalogfactory;

DefaultReplayerFactory::DefaultReplayerFactory()
{
//    addSupportedType("Camera");
    addSupportedType("DriveBicycle");
    addSupportedType("LaserScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("Odometry2d");
    addSupportedType("Odometry3d");
    addSupportedType("Power");
    addSupportedType("Wifi");
    addSupportedType("Gps");
}

orcalog::Replayer* 
DefaultReplayerFactory::create( const orcalog::LogReaderInfo &logReaderInfo )
{
    logReaderInfo.context.tracer()->debug( "Creating replayer: file="+logReaderInfo.filename+" type="+logReaderInfo.interfaceType+" fmt="+logReaderInfo.format, 5);

//     if ( logReaderInfo.interfaceType == "Camera" )
//     {
//         return new CameraReplayer( logReaderInfo );
//     }
    if ( logReaderInfo.interfaceType == "DriveBicycle" )
    {
        return new DriveBicycleReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "LaserScanner2d" )
    {
        return new LaserScanner2dReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Localise2d" )
    {
        return new Localise2dReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Localise3d" )
    {
        return new Localise3dReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Odometry2d" )
    {
        return new Odometry2dReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Odometry3d" )
    {
        return new Odometry3dReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Power" )
    {
        return new PowerReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Wifi" )
    {
        return new WifiReplayer( logReaderInfo );
    }
    else if ( logReaderInfo.interfaceType == "Gps" )
    {
        return new GpsReplayer( logReaderInfo );
    }
    else
    {
        return NULL;
    }
}

orcalog::ReplayerFactory* createReplayerFactory()
{
    return new DefaultReplayerFactory;
}
