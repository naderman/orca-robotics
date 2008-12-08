/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QDebug>
#include <cmath>

#include "viewwidget.h"
#include "imagequeue.h"

using namespace imageviewqt;

ViewWidget::ViewWidget(ImageQueue* imageQueue, QWidget* parent)
: QGLWidget(parent)
, imageQueue_(imageQueue)
, texture_(0)
, textureInitialized_(false)
{
    timer_.start();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

ViewWidget::~ViewWidget()
{
    delete texture_;
}   

void
ViewWidget::initializeGL()
{
    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
        throw gbxutilacfr::Exception(ERROR_INFO, "GlEW failed to initialize");
    }

    if( !GLEW_VERSION_2_0 )
    {
        throw gbxutilacfr::Exception(ERROR_INFO, "OpenGL version too old, need 2.0");
    }

    // drawing options
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    // features
    glEnable(GL_TEXTURE_RECTANGLE_ARB);

    // matrices
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640, 0.0, 480);

}


void 
ViewWidget::resizeGL(int width, int height)
{
    // matrices
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
}

void
ViewWidget::paintGL()
{

    updateTexture();

    if(!textureInitialized_)
    {
        return;
    }

    glActiveTexture(GL_TEXTURE0_ARB);
    texture_->pushBind();

    glBegin(GL_QUADS);
    
    glTexCoord2f( TEXCOORD_TL(texture_) );
    glVertex2f( 0.0, 0.0 );
    
    glTexCoord2f( TEXCOORD_TR(texture_) );
    glVertex2f( width(), 0.0 );
    
    glTexCoord2f( TEXCOORD_BR(texture_) );
    glVertex2f( width(), height() );
    
    glTexCoord2f( TEXCOORD_BL(texture_) );
    glVertex2f( 0.0, height() );
    
    glEnd();
    texture_->popBind();

    glFlush();

}

void
ViewWidget::initializeVertices()
{

}

void 
ViewWidget::updateVertices()
{

}

void
ViewWidget::flipX(bool flipX)
{
    flipX_ = flipX;
    updateVertices();
}

void
ViewWidget::flipY(bool flipY)
{
   flipY_ = flipY;
   updateVertices();
}

void
ViewWidget::fixedSize(bool fixedSize)
{
    fixedSize_ = fixedSize;
    updateVertices();
}

void 
ViewWidget::zoom(double zoom)
{
    zoom_ = zoom;
    updateVertices();
    emit(zoomChanged(zoom_));
}

void
ViewWidget::initializeTexture(uint32_t width, uint32_t height, std::string format)
{
    if(texture_)
    {
        delete texture_;
    }

    texture_ = new hydroglu::GLTexture("Image Stream", width, height, GL_TEXTURE_RECTANGLE_ARB, formatFromString(format), false);

    //set texture state vars
    textureInitialized_ = true;
}

void
ViewWidget::updateTexture()
{
    if(imageQueue_->isEmpty())
    {
        return;
    }
    
    orca::ImageDataPtr image = imageQueue_->pop();
    orca::ImageDescriptionPtr descr = image->description;

    if( !textureInitialized_ 
        || texture_->width() != static_cast<uint32_t>(descr->width)
        || texture_->height() != static_cast<uint32_t>(descr->height)
        || texture_->format() != formatFromString(descr->format)
      )
    {
        initializeTexture(descr->width, descr->height, descr->format);
    }

    // update the texture with the latest buffer

    texture_->upload( &(image->data[0]), image->data.size(), formatFromString(descr->format) );

    int timediff = timer_.restart();
    double thisfps = 1000.0/(double)(timediff);
    emit(fpsChanged(thisfps));
}


GLenum
ViewWidget::formatFromString(std::string format)
{
    if( format == "BGR8" )
    {
        return GL_BGR;
    }
    else if( format == "RGB8" )
    {
        return GL_RGB;
    }
    else if( format == "RGBA8" )
    {
        return GL_RGBA;
    }
    else if( format == "BGRA" )
    {
        return GL_BGRA;
    }
    else if( format == "GRAY8" )
    {
        return GL_LUMINANCE;
    }
    else
    {
        return GL_LUMINANCE;
    }
}
