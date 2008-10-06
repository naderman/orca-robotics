#if 0
/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 20.00.04-20.00.07 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "glutil.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>
#include <gbxutilacfr/mathdefs.h>
#include <gbxutilacfr/exceptions.h>
#include <hydroqguielementutil/cov2d.h>

using namespace std;

namespace orcaqgui3d {

namespace glutil {

void
transform( float x,
           float y,
           float z,
           float roll,
           float pitch,
           float yaw )
{
    // cout<<"TRACE(glutil.cpp): transform("<<x<<","<<y<<","<<z<<","<<RAD2DEG(roll)<<","<<RAD2DEG(pitch)<<","<<RAD2DEG(yaw)<<")" << endl;

    glTranslatef( x, y, z );
    glRotatef( RAD2DEG(yaw),   0.0, 0.0, 1.0 );
    glRotatef( RAD2DEG(pitch), 0.0, 1.0, 0.0 );
    glRotatef( RAD2DEG(roll),  1.0, 0.0, 0.0 );
}

void
drawBox( float d, float w, float h, bool drawSurfaces, bool drawWireFrame )
{
    int type;
    for ( int i=0; i < 2; i++ )
    {
        if ( i==0 )
        {
            // surfaces
            if ( !drawSurfaces ) continue;
            type = GL_QUADS;
        }
        else
        {
            // outline
            if ( !drawWireFrame ) continue;
            glColor4f( 0.0, 0.0, 0.0, 1.0 );
            type = GL_LINE_LOOP;
        }

        if ( i==0 )
        {
            glBegin(type);
            glNormal3f( 0.0, 1.0, 0.0);
            glVertex3f( d, w,-h);	// Top Right Of The Quad (Top)
            glVertex3f(-d, w,-h);	// Top Left Of The Quad (Top)
            glVertex3f(-d, w, h);	// Bottom Left Of The Quad (Top)
            glVertex3f( d, w, h);	// Bottom Right Of The Quad (Top)
            glEnd();

            glBegin(type);
            glNormal3f( 0.0,-1.0, 0.0);
            glVertex3f( d,-w, h);	// Top Right Of The Quad (Bottom)
            glVertex3f(-d,-w, h);	// Top Left Of The Quad (Bottom)
            glVertex3f(-d,-w,-h);	// Bottom Left Of The Quad (Bottom)
            glVertex3f( d,-w,-h);	// Bottom Right Of The Quad (Bottom)
            glEnd();

            glBegin(type);
            glNormal3f( 0.0, 0.0, 1.0);
            glVertex3f( d, w, h);	// Top Right Of The Quad (Front)
            glVertex3f(-d, w, h);	// Top Left Of The Quad (Front)
            glVertex3f(-d,-w, h);	// Bottom Left Of The Quad (Front)
            glVertex3f( d,-w, h);	// Bottom Right Of The Quad (Front)
            glEnd();
        
            glBegin(type);
            glNormal3f( 0.0, 1.0,-1.0);
            glVertex3f( d,-w,-h);	// Top Right Of The Quad (Back)
            glVertex3f(-d,-w,-h);	// Top Left Of The Quad (Back)
            glVertex3f(-d, w,-h);	// Bottom Left Of The Quad (Back)
            glVertex3f( d, w,-h);	// Bottom Right Of The Quad (Back)
            glEnd();
        }

        glBegin(type);
        glNormal3f(-1.0, 1.0, 0.0);
        glVertex3f(-d, w, h);	// Top Right Of The Quad (Left)
        glVertex3f(-d, w,-h);	// Top Left Of The Quad (Left)
        glVertex3f(-d,-w,-h);	// Bottom Left Of The Quad (Left)
        glVertex3f(-d,-w, h);	// Bottom Right Of The Quad (Left)
        glEnd();
        
        glBegin(type);
        glNormal3f( 1.0, 1.0, 0.0);
        glVertex3f( d, w,-h);	// Top Right Of The Quad (Right)
        glVertex3f( d, w, h);	// Top Left Of The Quad (Right)
        glVertex3f( d,-w, h);	// Bottom Left Of The Quad (Right)
        glVertex3f( d,-w,-h);	// Bottom Right Of The Quad (Right)
        glEnd();

        if ( i==1 )
        {
            glBegin(GL_LINES);
            glVertex3f( d, w, h);
            glVertex3f(-d, w, h);

            glVertex3f( d,-w, h);
            glVertex3f(-d,-w, h);

            glVertex3f( d, w,-h);
            glVertex3f(-d, w,-h);

            glVertex3f( d,-w,-h);
            glVertex3f(-d,-w,-h);
            glEnd();
        }
    }
}

#define X .525731112119133606 
#define Z .850650808352039932

static GLfloat vdata[12][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

static GLint tindices[20][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };


void 
drawIcosahedron()
{
    for ( int i = 0; i < 20; i++) {    
        /* color information here */ 
        glBegin(GL_TRIANGLES);    
            glVertex3fv(&vdata[tindices[i][0]][0]); 
            glVertex3fv(&vdata[tindices[i][1]][0]); 
            glVertex3fv(&vdata[tindices[i][2]][0]);    
        glEnd(); 
    }
}

void makeCheckImage64x64x3( GLubyte img[64][64][3],
                            int numSquaresPerEdge,
                            int lowVal, 
                            int highVal )
{
    const int widthInPixels=64;
    const int heightInPixels=64;
    assert( lowVal >= 0 && lowVal <= 255 );
    assert( highVal >= 0 && highVal <= 255 );

    int wOn=0;
    int hOn=0;
    for (int i = 0; i < widthInPixels; i++) 
    {
        if ( (i % (widthInPixels/numSquaresPerEdge)) == 0 )
            wOn = wOn ? 0 : 1;

        for (int j = 0; j < heightInPixels; j++) 
        {
            if ( (j % (heightInPixels/numSquaresPerEdge)) == 0 )
                hOn = hOn ? 0 : 1;

            int c = (wOn^hOn);
            if ( c==0 ) c = lowVal;
            else c = highVal;
            // cout<<"TRACE(glutil.cpp): hOn: " << hOn << ", wOn: " << wOn << ", c: " << c << endl;
            img[i][j][0] = (GLubyte) c;
            img[i][j][1] = (GLubyte) c;
            img[i][j][2] = (GLubyte) c;
        }
    }
}

std::string
errorToString( GLenum error )
{
    switch ( error )
    {
    case GL_NO_ERROR          : return "GL_NO_ERROR";
    case GL_INVALID_ENUM      : return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE     : return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION : return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW    : return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW   : return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY     : return "GL_OUT_OF_MEMORY";
    default: 
    {
        stringstream ss;
        ss << "Unknown("<<error<<")";
        return ss.str();
    }
    }
}

void
checkGLError()
{
    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        stringstream ss;
        ss << "OpenGL Error: " << errorToString(error);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

ScopedMatrixSave::ScopedMatrixSave()
{
    glPushMatrix();
}

ScopedMatrixSave::~ScopedMatrixSave()
{
    glPopMatrix();
}

void paintCovarianceEllipse( float pxx, float pxy, float pyy )
{
    // Quick checks first (note that this is a necessary but not
    // sufficient condition for positive-definiteness)
    if ( pxx < 0.0 ||
         pyy < 0.0 ||
         fabs(pxy) >= sqrt( pxx*pyy ) ||
         (isnan(pxx)||isinf(pxx))    ||
         (isnan(pxy)||isinf(pxy))    ||
         (isnan(pyy)||isinf(pyy))    )
    {
        std::stringstream ss;
        ss << "paintCovarianceEllipse(): covariance matrix not PD: pxx,pxy,pyy = "<<pxx<<","<<pxy<<","<<pyy;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // Work out the details of the uncertainty ellipse
    double a, b, psi;
    hydroqguielementutil::Cov2d cov( pxx, pxy, pyy );
    cov.ellipse( a, b, psi );

    {
        ScopedMatrixSave sms;
        glRotatef( psi, 0.0, 0.0, 1.0 );
        drawEllipse( a, b );
    }
}

void 
drawEllipse( float radiusX, float radiusY, int numPts )
{
    glBegin( GL_LINE_LOOP );
        
    float angle=0;
    for ( int i=0; i < numPts; i++ )
    {
        glVertex2f( radiusX*cos(angle), radiusY*sin(angle) );
        angle += 2*M_PI/(float)(numPts);
    }

    glEnd();
}

void drawCyclinder( float height, float radiusX, float radiusY, int numFacets )
{
    glBegin( GL_QUADS );

    float angle=0;
    for ( int i=0; i < numFacets; i++ )
    {
        float nextAngle = angle + 2*M_PI/(float)(numFacets);

        glVertex3f( radiusX*cos(angle), radiusY*sin(angle), 0.0 );
        glVertex3f( radiusX*cos(nextAngle), radiusY*sin(nextAngle), 0.0 );
        glVertex3f( radiusX*cos(nextAngle), radiusY*sin(nextAngle), height );
        glVertex3f( radiusX*cos(angle), radiusY*sin(angle), height );

        angle = nextAngle;
    }    

    glEnd();
}

} // namespace
} // namespace
#endif
