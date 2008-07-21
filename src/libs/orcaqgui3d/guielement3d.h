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
//        : needsInit_(true)
        {}

    virtual ~GuiElement3d() {}

    //! Paint is called periodically to paint the internal state of the guielement
    //! (The view gives information about stuff like the camera position,
    //!  the QGLWidget allows things like drawing text)
    virtual void setCameraPose( const CoordinateFrame &cameraPose ) {};

//     //! This is a special function: if we have to do OpenGL initialisation stuff,
//     //! we can't do it in the constructor (We might not be in the OpenGL context).
//     //! Have to do it here.
//     virtual void init( const View &v ) {}
//     //! This is a special function: if we have to do OpenGL destruction stuff,
//     //! we can't do it in the destructor (We might not be in the OpenGL context).
//     //! Have to do it here.
//     virtual void finit( const View &v ) {}

    //! Returns the OpenSceneGraph Node
    virtual osg::Node *osgNode() const=0;

//     //! Since there's init/finit stuff we can't do in the
//     //! constructor/destructor, need to track it here.
//     // TODO: How to handle the finit gear ???
//     bool needsInit() const { return needsInit_; }

//     // Call this once the thing has been initialised
//     void initialisationDone() { needsInit_ = false; }

private: 

//     bool needsInit_;
};

}

#endif
