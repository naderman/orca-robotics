/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 20.00.04-20.00.07 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "glutil.h"
#include <iostream>

#include <GL/glut.h>
#include <orcaobj/mathdefs.h>

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
            glBegin(GL_QUADS);
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

ScopedMatrixSave::ScopedMatrixSave()
{
    glPushMatrix();
}

ScopedMatrixSave::~ScopedMatrixSave()
{
    glPopMatrix();
}

} // namespace
} // namespace
