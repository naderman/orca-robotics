/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OGMAPLOADER_MAPLOAD_H
#define OGMAPLOADER_MAPLOAD_H

#include <iostream>
#include <orcaice/context.h>
#include <orca/ogmap.h>

namespace ogmaploader {

typedef enum {
    ICE_STREAM,
    PNM_GZ,
    BITMAP
} FileType;

// Class to load an occupancy grid from a file
class MapLoader
{
public:
    
    MapLoader( orcaice::Context context,
               Ice::PropertiesPtr prop, 
               std::string prefix );

    ~MapLoader();
    
    void loadMapFromFile( orca::OgMapDataPtr &map );

private:

    orcaice::Context context_;
    FileType type_;
    
    std::string filename_;
    float worldSizeX_;
    float worldSizeY_;
    float originX_;
    float originY_;
    float originTheta_;
    bool  negate_;
    
    int loadIceStream( orca::OgMapDataPtr &map );
    void setMapParameters( orca::OgMapDataPtr &map );
    
};

}

#endif
