/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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
    LaserScanner2dPrx laserPrx;

    try
    {
        orcaice::connectToInterfaceWithString( context_, laserPrx, listener_.interfaceName() );

        RangeScanner2dDescription descr;
        descr = laserPrx->getDescription();
        painter_.setDescription( descr );
    }
    catch ( hydroutil::Exception &e )
    {
        cout<<"TRACE(laserelement.cpp): got exception :"<<e.what()<<endl;
        // Ignore it.  We'll try reconnecting later.
    }
    catch ( Ice::Exception &e )
    {
        cout<<"TRACE(laserelement.cpp): got Ice exception :"<<e<<endl;
        // Ignore it.  We'll try reconnecting later.
    }
    catch ( std::exception &e )
    {
        cout<<"TRACE(laserelement.cpp): got std exception :"<<e.what()<<endl;
        // Ignore it.  We'll try reconnecting later.
    }
    catch ( ... )
    {
        cout<<"TRACE(laserelement.cpp): got some exception"<<endl;
        // Ignore it.  We'll try reconnecting later.
    }
}

QStringList
LaserScanner2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Scan"<<"Toggle Walls"<<"Toggle Reflectors";
    return s;
}


}
