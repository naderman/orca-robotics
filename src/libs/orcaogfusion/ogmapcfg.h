/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OG_MAP_CFG_H
#define OG_MAP_CFG_H

namespace ogfusion{

typedef struct
{
    double mapResX;
    double mapResY;
    int mapSizeX;
    int mapSizeY;
    double mapOriginX;
    double mapOriginY;
    double mapOrientation;

}MapConfig;

}
#endif
