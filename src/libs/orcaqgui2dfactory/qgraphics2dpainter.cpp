/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <assert.h>
#include <QPainter>

#include <orcaice/orcaice.h>

#include "qgraphics2dpainter.h"
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui/exceptions.h>

using namespace std;
using namespace orca;
using namespace orcaqgui;

QGraphics2dPainter::QGraphics2dPainter()
    : isDataAvailable_(false)
{
}

QGraphics2dPainter::~QGraphics2dPainter()
{
}

void
QGraphics2dPainter::clear()
{
    data_.qpicture.clear();
    qPicture_.setData( 0, 0 );
    isDataAvailable_ = false;
}


void
QGraphics2dPainter::setData( const orca::QGraphics2dData& data )
{
    if ( data.z < 1 || data.z > 12 )
        throw orcaqgui::Exception( "QGraphics2dPainter::setData(): invalid z" );

    data_ = data;
    isDataAvailable_ = true;

    qPicture_.setData( (char*)(&(data_.qpicture[0])), data_.qpicture.size() );
}

bool
QGraphics2dPainter::paintThisLayer(int z) const
{
    if ( !isDataAvailable_ ) return false;
    if ( z != data_.z ) return false;
    if ( data_.qpicture.size() == 0 ) return false;
    return true;
}

void
QGraphics2dPainter::paint( QPainter *painter, int z )
{
    if ( !isDataAvailable_ ) return;
    if ( z != data_.z ) return;
    if ( data_.qpicture.size() == 0 ) return;

    painter->save();
    {
        double scaleX = qPicture_.logicalDpiX() / (double) painter->device()->logicalDpiX();
        double scaleY = qPicture_.logicalDpiY() / (double) painter->device()->logicalDpiY();
        painter->scale(scaleX, scaleY);

        qPicture_.play( painter );
    }
    painter->restore();
}
