/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <sstream>

#include "gridelement.h"
#include <GL/glut.h>

using namespace std;
using namespace orcaqgui3d;

GridElement::GridElement()
    : isDisplayGrid_(true),
      isDisplayOrigin_(true),
      isDisplayLabels_(true)
{
}

void
GridElement::paint( QGLWidget *p )
{
//     cout<<"TRACE(gridelement3d.cpp): TODO: remove early return." << endl;
//     return;

    if ( isDisplayOrigin_ ) {
        //cout<<"painting origin"<<endl;
        drawOrigin();
    }

    if ( isDisplayGrid_ )
        drawGridAndLabels( p );

    if ( isDisplayOrigin_ )
        drawOrigin();
}

void
GridElement::drawGridAndLabels( QGLWidget *p )
{
    // Major line every N lines (others are minor)
    const int majorEveryN=2;
    const int numMajor=3;
    const float gridWidth=6; // [m]

    int gridLinesEachDirn=numMajor*majorEveryN;

//    cout<<"TRACE(grid3d.cpp): zoomFactor: " << guiGLPainter_->zoomFactor_ << endl;

    float w = gridWidth/2.0;

    for(int i=-gridLinesEachDirn; i <= gridLinesEachDirn; i++)
    {
        float d = gridWidth*(float)i/(float)(2*gridLinesEachDirn);
        if(i%(majorEveryN)==0)
        {
            glColor4f(0,0,0,0.5);

            if ( isDisplayLabels_ )
            {
                if ( i != 0 )
                {
                    std::stringstream ss;
                    ss << (int)d;
                    p->renderText( d, 0, 0, ss.str().c_str() );
                    p->renderText( 0, d, 0, ss.str().c_str() );
                }
            }
        }
        else 
        {
            glColor4f(0.5,0.5,0.5,0.5);
        }

        glBegin(GL_LINES);
        glVertex3f(-w, d, 0);
        glVertex3f(w,  d, 0);
        glVertex3f(d, -w, 0);
        glVertex3f(d,  w, 0);
        glEnd();
    }
}

void
GridElement::drawOrigin()
{
    // Draw axes on origin
    const float scaleAxes=1.0;
    const float d = 0.2*scaleAxes;
    glBegin(GL_LINES);
    glColor4f(0,0,1,1);
    glVertex3f(0,0,0);
    glVertex3f(d,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,d,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,d);

    const float p=0.1*scaleAxes;
    // x
    glVertex3f(d+p  ,p/2.0,0);
    glVertex3f(d+2*p,-p/2.0,0);
    glVertex3f(d+p  ,-p/2.0,0);
    glVertex3f(d+2*p,p/2.0,0);

    // y
    glVertex3f(-p/2.0,d+p,0);
    glVertex3f(p/2.0 ,d+2*p,0);
    glVertex3f(-p/2.0,d+2*p,0);
    glVertex3f(0     ,d+1.5*p,0);

    // z
    glVertex3f(-p/2.0,0,d+p);
    glVertex3f(p/2.0, 0,d+p);
    glVertex3f(-p/2.0,0,d+p);
    glVertex3f(p/2.0, 0,d+2*p);
    glVertex3f(-p/2.0,0,d+2*p);
    glVertex3f(p/2.0, 0,d+2*p);

    glEnd();
}

QStringList
GridElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Grid"<<"Toggle Origin"<<"Toggle Labels";
    return s;
}

void
GridElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
        // toggle grid
        isDisplayGrid_ = !isDisplayGrid_;
        break;
    case 1 :
        // toggle origin
        isDisplayOrigin_ = !isDisplayOrigin_;
        break;
    case 2 :
        // toggle origin
        isDisplayLabels_ = !isDisplayLabels_;
        break;
    }
}

