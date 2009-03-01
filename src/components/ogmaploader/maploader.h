/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAPLOAD_H
#define MAPLOAD_H

#include <iostream>
#include <orcaice/context.h>
#include <orca/ogmap.h>

namespace ogmaploader {

void loadMapFromFile( const orcaice::Context & context, orca::OgMapData& map );

}

#endif
