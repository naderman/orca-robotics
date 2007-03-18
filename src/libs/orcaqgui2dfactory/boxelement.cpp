/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <qpainter.h>
#include "boxelement.h"
#include <orcaqgui2d/definitions2d.h>

namespace orcaqgui2d {

BoxElement::BoxElement( QColor color, float centreX, float centreY, float width, float height )
    : color_(color),
      centreX_(centreX),
      centreY_(centreY),
      width_(width),
      height_(height)
{
}

BoxElement::~BoxElement()
{
}

void BoxElement::paint( QPainter* p, const int z )
{
    if ( z != Z_GRID ) return;

    p->save();
    {
        p->setPen( color_ );
        p->setBrush( color_ );

        p->drawRect( QRectF( centreX_-width_/2.0, 
                             centreY_-height_/2.0,
                             width_,
                             height_ ) );
    }
    p->restore();
}

}
