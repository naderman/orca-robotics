/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>
#include <fstream>

#include "maploadutil.h"
#include "maploader.h"

using namespace std;
using namespace orca;

namespace ogmaploader {

MapLoader::MapLoader(   orcaice::Context context,
                        Ice::PropertiesPtr prop, 
                        std::string prefix )
    : context_(context)
{
    filename_   = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );
    worldSizeX_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.X", 20.0 );
    worldSizeY_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.Y", 20.0 );
    originX_    = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Origin.X", 0.0 );
    originY_    = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Origin.Y", 0.0 );
    originTheta_= orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Origin.Orientation", 0.0 ) * M_PI/180.0;
    negate_     = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Negate", true );
    
    int len = strlen( filename_.c_str() );
    
    if ( strcmp( &filename_.c_str()[len - 4], ".bin" ) == 0 )
    {
        type_= ICE_STREAM;
    }
    else if( strcmp( &filename_.c_str()[len - 7], ".pnm.gz" ) == 0 )
    {
        type_= PNM_GZ;
    }
    else
    {
        type_ = BITMAP;    
    }
    
}
    
void 
MapLoader::loadMapFromFile( orca::OgMapDataPtr &map )
{
    switch (type_)
    {
        case PNM_GZ:        if (maploadutil::loadPnmGz( filename_.c_str(), negate_, map->numCellsX, map->numCellsY, map->data ) != 0)
                            {
                                stringstream ss;
                                ss << "ERROR(maploader.cpp): failed to open " << filename_;
                                throw ss.str();
                            }
                            setMapParameters( map );      
                            break;
                        
        case BITMAP:        if (maploadutil::loadBitmap( filename_.c_str(), negate_, map->numCellsX, map->numCellsY, map->data ) != 0)
                            {
                                stringstream ss;
                                ss << "ERROR(maploader.cpp): failed to open " << filename_;
                                throw ss.str();
                            }
                            setMapParameters( map );     
                            break;
                            
        case ICE_STREAM:    if ( loadIceStream( map ) != 0 )
                            {
                                stringstream ss;
                                ss << "ERROR(maploader.cpp): failed to open " << filename_;
                                throw ss.str();
                            }
                            break;
    }
}

void
MapLoader::setMapParameters( OgMapDataPtr &map )
{
        map->origin.p.x = originX_;
        map->origin.p.y = originY_;
        map->origin.o   = originTheta_;
    
        // since we know that map size in pixels, we can calculate the cell size
        map->metresPerCellX = worldSizeX_ / (float)map->numCellsX;
        map->metresPerCellY = worldSizeY_ / (float)map->numCellsY;    
}

int
MapLoader::loadIceStream( OgMapDataPtr &map )
{
    orca::OgMapDataPtr ogMap = new orca::OgMapData;
        
    std::ifstream *logFile = new std::ifstream( filename_.c_str(), ios::binary|ios::in );
                    
    std::vector<Ice::Byte> byteData;
    size_t length;
    logFile->read( (char*)&length, sizeof(length) );
    if ( logFile->bad() )
    {
        context_.tracer()->error("logFile is bad");
        return -1;
    }
    
    byteData.resize( length );
    logFile->read( (char*)&byteData[0], length );
    if ( logFile->bad() )
    {
        context_.tracer()->error("logFile is bad");
        return -1;
    }
    
    Ice::InputStreamPtr iceInputStreamPtr = Ice::createInputStream( context_.communicator(), byteData );
    if ( !byteData.empty() )
    {
        ice_readOgMapData( iceInputStreamPtr, map );
        iceInputStreamPtr->readPendingObjects();
    }
    
    return 0;
    
}

}

