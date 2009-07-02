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
#include <hydroqgui/exceptions.h>
#include "pointcloudelement.h"

using namespace std;

namespace orcaqgui3d {

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
