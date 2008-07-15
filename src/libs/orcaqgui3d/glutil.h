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

#include <GL/gl.h>
#include <string>

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

// Makes a little grayscale checkerboard.
void makeCheckImage64x64x3( GLubyte img[64][64][3],
                            int numSquaresPerEdge,
                            int lowVal=0,
                            int highVal=255 );

std::string errorToString( GLenum error );

// Throws an exception if there's an error condition
void checkGLError();

//
// Calls glPushMatrix() in the constructor, and glPopMatrix() in the destructor.
//
class ScopedMatrixSave
{
public:

    ScopedMatrixSave();
    ~ScopedMatrixSave();
};

//! Paints 2-sigma ellipse for the position uncertainty
void paintCovarianceEllipse( float pxx, float pxy, float pyy );

//! draws an axis-aligned ellipse
void drawEllipse( float radiusX, float radiusY, int numPts=25 );

//! draws a cylinder oriented in the +z direction
void drawCyclinder( float height, float radiusX, float radiusY, int numFacets=25 );

} // namespace
} // namespace

#endif
