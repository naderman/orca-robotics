/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orca/pathplanner2d.h>
#include <hydroqguielementutil/paintutils.h>
#include <orcaqgui2dfactory/pathconversionutil.h>

#include "pathplannerpainter.h"

using namespace std;

namespace orcaqgui2d {

PathPlannerPainter::PathPlannerPainter()
    : wpIndex_(-1),
      useTransparency_(true),
      displayWaypoints_(true),
      color_(Qt::blue),
      inFocus_(true)
{
}

void PathPlannerPainter::setData( const orca::PathPlanner2dData& path )
{
    orcaPathToGuiPath( path.path, guiPath_ );
}

void PathPlannerPainter::paint( QPainter *painter, int z )
{
    if ( z != hydroqguielementutil::Z_PATH ) return;
    if ( !displayWaypoints_ ) return;

    QColor wpColor = color_;
    if (useTransparency_)
        wpColor = hydroqguielementutil::getTransparentVersion(color_,0.7);

    // === drawing individual waypoints ===
    for ( int i=0; i < (int)guiPath_.size(); i++)
        hydroqguipath::drawWaypoint( painter, guiPath_, i, wpColor, wpColor );

    // === draw connections between waypoints ===
    hydroqguipath::drawWpConnections( painter, guiPath_, wpColor, 0, guiPath_.size() );

}

}


