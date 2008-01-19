/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_GUIELEMENT3D_H
#define ORCAGUI3D_GUIELEMENT3D_H

#include <QGLWidget>
#include <orcaqgui/guielement.h>

namespace orcaqgui3d {

//!
//! @author Alex Brooks
//!
class GuiElement3d : public orcaqgui::GuiElement
{

public: 

    virtual ~GuiElement3d() {}

    //! Paint is called periodically to paint the internal state of the guielement
    virtual void paint( QGLWidget *p ) {};

private: 


};

}

#endif
