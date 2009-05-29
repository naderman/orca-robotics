/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_GRIDELEMENT_H
#define ORCAGUI_GRIDELEMENT_H

#include <hydroqguielementutil/definitions2d.h>
#include <hydroqguielementutil/guielement2d.h>

namespace orcaqgui2d
{

/*!
  @brief Paints a rectangular grid in the display.
  @author Tobias Kaupp, Alex Brooks, Ben Upcroft
*/  
class GridElement : public hydroqguielementutil::GuiElement2d
{
public:

	GridElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo );
	
    virtual bool isInGlobalCS() { return true; }
    
	virtual void paint( QPainter *p, int z );
    virtual bool paintThisLayer(int z) const { return z==hydroqguielementutil::Z_GRID || z==hydroqguielementutil::Z_ORIGIN; }

    virtual QStringList contextMenu();
    
    virtual void execute( int action );

    void setDisplayGrid( bool display ) { isDisplayGrid_ = display; }
    void setDisplayOrigin( bool display ) { isDisplayOrigin_ = display; }
    void setDisplayLabels( bool display ) { isDisplayLabels_ = display; }

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
