/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "maploader.h"
#include <orcaice/orcaice.h>
#include <fstream>

#include <hydromapload/hydromapload.h>

using namespace std;
using namespace orca;

namespace ogmaploader {
    
int
loadIceStream( const orcaice::Context & context, std::string filename, OgMapData& map )
{
    std::ifstream *logFile = new std::ifstream( filename.c_str(), ios::binary|ios::in );
    if ( !logFile->is_open() )
    {
        context.tracer().error("logFile could not be loaded");
        return -1;
    }
                    
    std::vector<Ice::Byte> byteData;
    size_t length;
    logFile->read( (char*)&length, sizeof(length) );
    if ( logFile->good() == false )
    {
        context.tracer().error("reading from logFile failed");
        return -1;
    }
    
    byteData.resize( length );
    logFile->read( (char*)&byteData[0], length );
    if ( logFile->good() == false )
    {
        context.tracer().error("reading from logFile failed");
        return -1;
    }
    
    Ice::InputStreamPtr iceInputStreamPtr = Ice::createInputStream( context.communicator(), byteData );
    if ( !byteData.empty() )
    {
        ice_readOgMapData( iceInputStreamPtr, map );
        iceInputStreamPtr->readPendingObjects();
    }
    
    return 0;
    
}

void 
loadMapFromFile( const orcaice::Context & context, orca::OgMapData& map )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag();
    prefix += ".Config.";

    std::string filename = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );
    int len = strlen( filename.c_str() );
    
    if ( strcmp( &filename.c_str()[len - 4], ".bin" ) == 0 )
    {
        //
        // Load an ICE stream
        //
        if ( loadIceStream( context, filename, map ) != 0 )
        {
            stringstream ss;
            ss << "ERROR(maploader.cpp): something went wrong while loading " << filename;
            throw ss.str();
        }
    }
    else
    {
        //
        // Load a normal image format
        //
        bool negate = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Negate", true );
        hydromapload::loadMap( filename.c_str(), negate, map.numCellsX, map.numCellsY, map.data );

        map.offset.p.x = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.X", 0.0 );
        map.offset.p.y = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.Y", 0.0 );
        map.offset.o   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.Orientation", 0.0 ) * M_PI/180.0;

        // since we know that map size in pixels, we can calculate the cell size
        float worldSizeX = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.X", 20.0 );
        float worldSizeY = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.Y", 20.0 );
        map.metresPerCellX = worldSizeX / (float)map.numCellsX;
        map.metresPerCellY = worldSizeY / (float)map.numCellsY;

        // Make up a timestamp
        map.timeStamp.seconds  = 0;
        map.timeStamp.useconds = 0;
        
        // Set type
        int mapType = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MapType", 0 );
        
        switch (mapType)
        {
            case 0: map.mapType = orca::OgMapOccupancy; break;
            case 1: map.mapType = orca::OgMapHazard; break;
            case 2: map.mapType = orca::OgMapMotion; break;
        }
    }
}

}

