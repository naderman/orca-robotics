/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_GRIDELEMENT_H
#define ORCAGUI_GRIDELEMENT_H

#include <hydroqgui/definitions2d.h>
#include <hydroqgui/guielement2d.h>

namespace orcaqgui2d
{

/*!
  @brief Paints a rectangular grid in the display.
  @author Tobias Kaupp, Alex Brooks, Ben Upcroft
*/  
class GridElement : public hydroqgui::GuiElement2d
{
public:

	GridElement();
	
    virtual bool isInGlobalCS() { return true; }
    
	virtual void paint( QPainter *p, int z );
    virtual bool paintThisLayer(int z) const { return z==hydroqgui::Z_GRID || z==hydroqgui::Z_ORIGIN; }

    virtual QStringList contextMenu();
    
    virtual void execute( int action );

protected:

    virtual bool isPermanentElement() const { return true; }
    
private:
	
    // Distance between grid lines [m]
	double step_;
    
    bool isDisplayGrid_;
    bool isDisplayOrigin_;
    bool isDisplayLabels_;
    
};

} // namespace

#endif
