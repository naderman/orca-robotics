/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_GRIDELEMENT_H
#define ORCAGUI_GRIDELEMENT_H

#include <orcaqgui2d/guielement2d.h>
#include <orcaqgui2d/definitions2d.h>
#include <QPainter>


namespace orcaqgui
{

/*!
  @brief Paints a rectangular grid in the display.
  @author Tobias Kaupp, Alex Brooks, Ben Upcroft
*/  
class GridElement : public GuiElement2d
{
public:

	GridElement();
	
    virtual bool isInGlobalCS() { return true; }
    
	virtual void paint( QPainter *p, int z );
    virtual bool paintThisLayer(int z) const { return z==Z_GRID || z==Z_ORIGIN; }

    virtual QStringList contextMenu();
    
    virtual void execute( int action );
    
private:
	
    // Distance between grid lines [m]
	double step_;
    
    bool isDisplayGrid_;
    bool isDisplayOrigin_;
    bool isDisplayLabels_;
    
};

} // namespace

#endif
