/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_GRIDELEMENT3D_H
#define ORCAGUI3D_GRIDELEMENT3D_H

#include <QGLWidget>
#include <orcaqgui/ipermanentelement.h>
#include <orcaqgui3d/guielement3d.h>

namespace orcaqgui3d
{

/*!
  @brief Paints a rectangular grid in the 3d display.
  @author Alex Brooks, Tobias Kaupp
*/  
class GridElement : public GuiElement3d, public orcaqgui::IPermanentElement
{
public:

    GridElement();
	
    virtual bool isInGlobalCS() { return true; }
    
    virtual void paint( QGLWidget *p );
    
    virtual QStringList contextMenu();
    
    virtual void execute( int action );
    
private:

    void drawGridAndLabels( QGLWidget *p );
    void drawOrigin();
	
    bool isDisplayGrid_;
    bool isDisplayOrigin_;
    bool isDisplayLabels_;
    
};

} // namespace

#endif
