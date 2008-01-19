/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_GLUTIL_H
#define ORCAGUI3D_GLUTIL_H

namespace orcaqgui3d {

namespace glutil {
        
void transform( float x,
                float y,
                float z,
                float roll,
                float pitch,
                float yaw );

// Draws a box centred on the origin.
// Draws the surfaces in the current colour
void drawBox( float d, float w, float h, bool drawSurfaces, bool drawWireFrame );

void drawIcosahedron();

//
// Calls glPushMatrix() in the constructor, and glPopMatrix() in the destructor.
//
class ScopedMatrixSave
{
public:

    ScopedMatrixSave();
    ~ScopedMatrixSave();
};

} // namespace
} // namespace

#endif
