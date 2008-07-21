/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orca/laserscanner2d.h>

#include "laserscanner2delement.h"

using namespace std;
using namespace orca;

namespace orcaqgui3d {

void
LaserScanner2dElement::getLaserInfo()
{
    // Subscribe directly to get geometry etc
    orca::LaserScanner2dPrx laserPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, laserPrx, listener_.interfaceName() );
    
    orca::RangeScanner2dDescription descr;
    descr = laserPrx->getDescription();

    painter_.setDescr( descr.offset, descr.size );
}


}
