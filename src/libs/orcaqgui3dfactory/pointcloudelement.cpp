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
#include <orca/pointcloud.h>

#include "pointcloudelement.h"

using namespace std;

namespace orcaqgui3d {

void
PointCloudElement::getLaserInfo()
{
    // Subscribe directly to get geometry etc
    //orca::PointCloudPrx pointCloudPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    //orcaice::connectToInterfaceWithString( context_, pointCloudPrx, listener_.interfaceName() );
    
    //orca::RangeScanner2dDescription descr;
    //descr = pointCloudPrx->getDescription();

    //painter_.setDescr( descr.offset, descr.size );
}

QStringList
PointCloudElement::contextMenu()
{
    QStringList s;
    s<<"Sorry, no options so far";
    return s;
}

void 
PointCloudElement::execute( int action )
{
    switch ( action ) {
        // no actions so far
        default: {
            throw hydroqgui::Exception( ERROR_INFO, "PointCloudElement::execute(): What the hell? bad action." );
            break;
        }
    }
}

}
