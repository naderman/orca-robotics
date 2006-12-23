/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>

#include "fakemaploader.h"

using namespace orca;
using namespace std;

namespace ogmaploader {

void fakeLoadMap( orca::OgMapData& theMap )
{
    orcaice::setSane( theMap );
}

}
