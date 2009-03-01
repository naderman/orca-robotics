/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_GUIELEMENT3D_H
#define ORCAGUI3D_GUIELEMENT3D_H

#include <hydroqguielementutil/iguielement.h>
#include <orcaqgui3d/coordinateframe.h>
#include <osg/Node>

namespace orcaqgui3d {

//!
//! @author Alex Brooks
//!
class GuiElement3d : public hydroqguielementutil::IGuiElement 
{

public: 

    GuiElement3d()
        {}

    virtual ~GuiElement3d() {}

    //! Tells the GuiElements about the camera location, in case it wants
    //! to adjust accordingly.
    virtual void setCameraPose( const CoordinateFrame &cameraPose ) {};

    //! Returns the OpenSceneGraph Node
    virtual osg::Node *osgNode() const=0;
};

}

#endif
