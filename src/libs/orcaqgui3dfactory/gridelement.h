/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_GRIDELEMENT3D_H
#define ORCAGUI3D_GRIDELEMENT3D_H

#include <orcaqgui3d/view.h>
#include <orcaqgui3d/guielement3d.h>
#include <GL/gl.h>

namespace orcaqgui3d
{

/*!
  @brief Paints a rectangular grid in the 3d display.
  @author Alex Brooks
*/  
class GridElement : public GuiElement3d
{
public:

    GridElement( double wireGridSpacing=2,
                 double groundPlaneSquareSpacing=1 );
	~GridElement();

    virtual bool isInGlobalCS() { return true; }
    
    virtual void init( const orcaqgui3d::View &view  );
    virtual void finit();
    virtual void paint( const orcaqgui3d::View &view, QGLWidget &p );
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
private:

    void drawGroundPlane( const orcaqgui3d::View &view );
    void drawWireGridAndLabels( const orcaqgui3d::View &view, QGLWidget &p );
    void drawOrigin();
	
    bool isDisplayWireGrid_;
    bool isDisplayGroundPlane_;
    bool isDisplayOrigin_;
    bool isDisplayLabels_;
    
    double wireGridSpacing_;
    double groundPlaneSquareSpacing_;

    GLubyte checkImage_[64][64][3];
    GLuint textureName_;
};

} // namespace

#endif
