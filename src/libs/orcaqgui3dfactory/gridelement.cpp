/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <sstream>

#include "gridelement.h"
#include <orcaqgui3d/glutil.h>

using namespace std;
using namespace orcaqgui3d;

GridElement::GridElement( double wireGridSpacing,
                          double groundPlaneSquareSpacing )
    : isDisplayWireGrid_(true),
      isDisplayGroundPlane_(true),
      isDisplayOrigin_(true),
      isDisplayLabels_(true),
      wireGridSpacing_(wireGridSpacing),
      groundPlaneSquareSpacing_(groundPlaneSquareSpacing)
{
}

GridElement::~GridElement()
{
    cout<<"TRACE(gridelement.cpp): Does this work in here???" << endl;
    finit();
}

void
GridElement::init( const orcaqgui3d::View &view )
{
    cout<<"TRACE(gridelement.cpp): init()" << endl;

    //
    // Load the checkerboard texture
    //

    glGenTextures( 1, &textureName_ );
    glBindTexture( GL_TEXTURE_2D, textureName_ );

    // Draw the chess-board in memory
    orcaqgui3d::glutil::makeCheckImage64x64x3( checkImage_, 2, 120, 130 );

    // Says how to read the texture in the next call
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create the texture map
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64,
                 0, GL_RGB, GL_UNSIGNED_BYTE, 
                 &checkImage_[0][0][0]);    
}

void
GridElement::finit()
{
    glDeleteTextures( 1, &textureName_ );    
}

void
GridElement::paint( const orcaqgui3d::View &view, QGLWidget &p )
{
    if ( isDisplayGroundPlane_ )
        drawGroundPlane( view );

    if ( isDisplayOrigin_ ) {
        //cout<<"painting origin"<<endl;
        drawOrigin();
    }
    
    if ( isDisplayWireGrid_ )
        drawWireGridAndLabels( view, p );
}

void
GridElement::drawGroundPlane( const orcaqgui3d::View &view )
{
    // cout<<"TRACE(worldview.cpp): "<<__func__ << endl;

    glPushMatrix();
    {
        const double infty=1000;
        const double metresPerTile=2*groundPlaneSquareSpacing_;
        const double texCoordExtreme=2*infty/metresPerTile;

//         // Centre the ground plane near the camera, so we can never walk off.
//         int tilesFromOriginX = view.cameraX() / metresPerTile;
//         int tilesFromOriginY = view.cameraY() / metresPerTile;
//         glTranslatef( tilesFromOriginX*metresPerTile,
//                       tilesFromOriginY*metresPerTile,
//                       0.0 );

        glColor3f( 0.5, 0.5, 0.5 );

        glEnable( GL_TEXTURE_2D );
        {
            glBindTexture( GL_TEXTURE_2D, textureName_ );

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

            glBegin(GL_QUADS);
            glNormal3f( 0.0, 0.0, 1.0);
            glTexCoord2f(0,0);
            glVertex3f(-infty,-infty,0);
            glTexCoord2f(0,texCoordExtreme); 
            glVertex3f(-infty,infty,0);
            glTexCoord2f(texCoordExtreme,texCoordExtreme); 
            glVertex3f(infty,infty,0);
            glTexCoord2f(texCoordExtreme,0); 
            glVertex3f(infty,-infty,0);
            glEnd();
        }
        glDisable( GL_TEXTURE_2D );
    }
    glPopMatrix();
}

void
GridElement::drawWireGridAndLabels( const orcaqgui3d::View &view, QGLWidget &p )
{
    const int xOffset = view.cameraX();
    const int yOffset = view.cameraY();

    glPushMatrix();
    {
        // Centre near the camera, slightly off the ground so it's visible
        // glTranslatef( xOffset, yOffset, 0.001 );
        glTranslatef( xOffset, yOffset, 0.0 );

        // Major line every N lines (others are minor)
        const int majorEveryN=2;
        const int numMajor=4;
        const float gridWidth=numMajor*majorEveryN*wireGridSpacing_; // [m]

        int gridLinesEachDirn=numMajor*majorEveryN;

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
                        std::stringstream ssX, ssY;
                        ssX << (int)d+xOffset;
                        ssY << (int)d+yOffset;
                        p.renderText( d, 0, 0, ssX.str().c_str() );
                        p.renderText( 0, d, 0, ssY.str().c_str() );
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
    glPopMatrix();
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
    s<<"Toggle Grid"<<"Toggle Origin"<<"Toggle Labels"<<"Toggle Ground Plane";
    return s;
}

void
GridElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
        // toggle grid
        isDisplayWireGrid_ = !isDisplayWireGrid_;
        break;
    case 1 :
        // toggle origin
        isDisplayOrigin_ = !isDisplayOrigin_;
        break;
    case 2 :
        // toggle labels
        isDisplayLabels_ = !isDisplayLabels_;
        break;
    case 3 :
        // toggle ground plane
        isDisplayGroundPlane_ = !isDisplayGroundPlane_;
        break;
    }
}

