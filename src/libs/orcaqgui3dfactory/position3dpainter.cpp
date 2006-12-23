/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <assert.h>
#include <QString>
#include <GL/glut.h>

#include <orcaice/orcaice.h>

#include "position3dpainter.h"

using namespace std;
using namespace orca;
using namespace orcaqgui3d;

Position3dPainter::Position3dPainter( const char *objectModelFilename )
    : objectModel_(NULL)
{
#if HAVE_LIB3DS
    objectModel_ = new ObjectModel(objectModelFilename);
#endif
    clear();
}

Position3dPainter::~Position3dPainter()
{
    if ( objectModel_ ) delete objectModel_;
}

void
Position3dPainter::clear()
{
    data_.pose.p.x = 0.0;
    data_.pose.p.y = 0.0;
    data_.pose.p.z = 0.0;
    data_.pose.o.r = 0.0;
    data_.pose.o.p = 0.0;
    data_.pose.o.y = 0.0;
}

// void
// Position3dPainter::setGeometry( orca::RangeScanner2dGeometryPtr & geom )
// {
//     geom_ = geom;
// }

void
Position3dPainter::setData( const orca::Position3dData& data )
{
    data_ = data;

    // debug
    std::cout << "position3dpainter.cpp: " << orcaice::toString(data_) << std::endl;
}

void
Position3dPainter::paint( QGLWidget *p )
{
//     if ( data_ == 0 ) return;

    cout<<"TRACE(position3dpainter.cpp): painting..." << endl;

#if HAVE_LIB3DS
    objectModel_->render();
#else
    // Just draw an arrow    
    const float f = 0.2;
    const float d      = 0.3*f;

    glColor4f( 0, 1, 0, 1 );

    // draw the arrow
    glBegin( GL_LINES );
    glVertex3f( -f, 0, 0 );
    glVertex3f( f,  0, 0 );
    glVertex3f( f-d, -d, 0 );
    glVertex3f( f,   0,  0 );
    glVertex3f( f-d, d,  0 );
    glVertex3f( f,   0,  0 );
    glEnd();
#endif
}
