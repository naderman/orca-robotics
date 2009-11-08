/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <hydroqguipath/pathutils.h>
#include <hydroqguielementutil/paintutils.h>

namespace hydroqguipath {

float straightLineDist( QPointF line )
{
    return sqrtf(line.x()*line.x() + line.y()*line.y());
};

void drawWaypoint( QPainter *painter,
                   const GuiPath &guiPath,
                   int index,
                   const QColor &fillColor,
                   const QColor &drawColor )
{

    if ( (int)guiPath.size() <= index )
    {
        std::cout << "size of guiPath, index: " << (int)guiPath.size() << ", " << index << std::endl;
        assert( false && "pathutils: guipath size is smaller than expected" );
    }
//     assert( (int)guiPath.size() > index && "pathutils: guipath size is smaller than expected" );

    assert( index >= 0 && "pathutils: index needs to be positive" );

    painter->save();
    painter->translate( guiPath[index].position.x(), guiPath[index].position.y() );    // move to point
    hydroqguielementutil::paintWaypoint( painter,
                                         fillColor,
                                         drawColor,
                                         guiPath[index].heading,
                                         guiPath[index].distanceTolerance,
                                         guiPath[index].headingTolerance );
    painter->restore();
}

void drawWpConnections( QPainter *painter,
                        const GuiPath &guiPath,
                        const QColor &color,
                        int startIndex,
                        int endIndex )
{
    if ( guiPath.size()<=1 ) return;

    assert( startIndex >= 0 );
    assert( endIndex <= (int)guiPath.size() );

    QPen pen( color );
    pen.setCosmetic( true );
    painter->setPen( pen );
    painter->setBrush ( Qt::NoBrush );

    for ( int i=startIndex; i<endIndex; ++i)
    {
        if (i != 0) painter->drawLine(guiPath[i].position,guiPath[i-1].position);
    }
}


}
