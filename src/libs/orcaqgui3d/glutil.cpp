/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
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
    glRotatef( RAD2DEG(yaw),   0, 0, 1 );
    glRotatef( RAD2DEG(pitch), 0, 1, 0 );
    glRotatef( RAD2DEG(roll),  1, 0, 0 );
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
            glColor4f( 0, 0, 0, 1.0 );
            type = GL_LINE_LOOP;
        }

        if ( i==0 )
        {
            glBegin(type);
            glNormal3f( 0, 1, 0);
            glVertex3f( d, w,-h);	// Top Right Of The Quad (Top)
            glVertex3f(-d, w,-h);	// Top Left Of The Quad (Top)
            glVertex3f(-d, w, h);	// Bottom Left Of The Quad (Top)
            glVertex3f( d, w, h);	// Bottom Right Of The Quad (Top)
            glEnd();

            glBegin(type);
            glNormal3f( 0,-1, 0);
            glVertex3f( d,-w, h);	// Top Right Of The Quad (Bottom)
            glVertex3f(-d,-w, h);	// Top Left Of The Quad (Bottom)
            glVertex3f(-d,-w,-h);	// Bottom Left Of The Quad (Bottom)
            glVertex3f( d,-w,-h);	// Bottom Right Of The Quad (Bottom)
            glEnd();

            glBegin(type);
            glNormal3f( 0, 0, 1);
            glVertex3f( d, w, h);	// Top Right Of The Quad (Front)
            glVertex3f(-d, w, h);	// Top Left Of The Quad (Front)
            glVertex3f(-d,-w, h);	// Bottom Left Of The Quad (Front)
            glVertex3f( d,-w, h);	// Bottom Right Of The Quad (Front)
            glEnd();
        
            glBegin(type);
            glNormal3f( 0, 1,-1);
            glVertex3f( d,-w,-h);	// Top Right Of The Quad (Back)
            glVertex3f(-d,-w,-h);	// Top Left Of The Quad (Back)
            glVertex3f(-d, w,-h);	// Bottom Left Of The Quad (Back)
            glVertex3f( d, w,-h);	// Bottom Right Of The Quad (Back)
            glEnd();
        }

        glBegin(type);
        glNormal3f(-1, 1, 0);
        glVertex3f(-d, w, h);	// Top Right Of The Quad (Left)
        glVertex3f(-d, w,-h);	// Top Left Of The Quad (Left)
        glVertex3f(-d,-w,-h);	// Bottom Left Of The Quad (Left)
        glVertex3f(-d,-w, h);	// Bottom Right Of The Quad (Left)
        glEnd();
        
        glBegin(type);
        glNormal3f( 1, 1, 0);
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

ScopedMatrixSave::ScopedMatrixSave()
{
    glPushMatrix();
}

ScopedMatrixSave::~ScopedMatrixSave()
{
    glPopMatrix();
}



    }
}
