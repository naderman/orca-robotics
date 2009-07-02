/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orca/rangescanner2d.h>
#include <hydroqgui/exceptions.h>
#include "rangescanner2delement.h"

using namespace std;

namespace orcaqgui3d {

void
RangeScanner2dElement::getRangeInfo()
{
    // Subscribe directly to get geometry etc
    orca::RangeScanner2dPrx rangePrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, rangePrx, listener_.interfaceName() );
    
    orca::RangeScanner2dDescription descr;
    descr = rangePrx->getDescription();

    painter_.setDescr( descr.offset, descr.size, descr.minRange, descr.maxRange );
}

QStringList
RangeScanner2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Display Points"<<"Toggle Display";
    return s;
}

void 
RangeScanner2dElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
    {
        painter_.toggleDisplayPoints();
        break;
    }
    case 1 :
    {
        painter_.toggleDisplayScan();
        break;
    }
    default:
    {
        throw hydroqgui::Exception( ERROR_INFO, "OgMapElement::execute(): What the hell? bad action." );
        break;
    }
    }
}

}
