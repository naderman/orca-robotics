/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <orcaobj/orcaobj.h>

#include "fakemaploader.h"

using namespace std;

namespace ogmaploader {

void fakeLoadMap( orca::OgMapData& theMap )
{
    orcaobj::setSane( theMap );
}

}
