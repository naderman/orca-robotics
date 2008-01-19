/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QRED_BOXELEMENT_H
#define QRED_BOXELEMENT_H

#include <orcaqgui2d/guielement2d.h>
#include <qcolor.h>
#include <orcaqgui2d/definitions2d.h>

namespace orcaqgui2d {

/*!
  \brief Draws a simple square on the map.
  \author Alex Brooks
*/  
class BoxElement : public GuiElement2d
{
public:
	BoxElement( QColor color, float centreX, float centreY, float width, float height );
	~BoxElement();
	
	void paint( QPainter*, int z );
    bool paintThisLayer( int z ) const { return z == Z_GRID; }
    bool isInGlobalCS() { return true; }

private:

    QColor color_;
    float centreX_;
    float centreY_;
    float width_;
    float height_;
};

}

#endif
