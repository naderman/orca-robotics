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

void loadMapFromFile( const orcaice::Context & context, orca::OgMapDataPtr &map );

}

#endif
