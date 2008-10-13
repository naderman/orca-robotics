#include <QDebug>
#include "viewwidget.h"

using namespace orcaimageview;

ViewWidget::ViewWidget(orcaice::PtrBuffer<orca::ImageDataPtr>* imageQueue, QWidget* parent)
: QGLWidget(parent)
, imageQueue_(imageQueue)
, texture_(0)
, textureInitialized_(false)
{
}

ViewWidget::~ViewWidget()
{
    if(textureInitialized_)
    {
        glDeleteTextures( 1, &texture_);
    }   
}   

void
ViewWidget::initializeGL()
{
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

    glActiveTexture(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture_);

    glBegin(GL_QUADS);
    
    glTexCoord2f( 0.0, 0.0 );
    glVertex2f( 0.0, 0.0 );
    
    glTexCoord2f( 0.0, textureHeight_ );
    glVertex2f( 0.0, height() );
    
    glTexCoord2f( textureWidth_, textureHeight_ );
    glVertex2f( width(), height() );
    
    glTexCoord2f( textureWidth_, 0.0 );
    glVertex2f( width(), 0.0 );
    
    glEnd();
    
    glFlush();

}

void
ViewWidget::initializeTexture(uint32_t width, uint32_t height, std::string format)
{
    // if the texture already exists delete it since something has changed
    if(textureInitialized_)
    {
        glDeleteTextures( 1, &texture_);
    }

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture_);
    
    //set filters appropriately
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    //set texenv to replace instead of the default modulate
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    //allocate graphics memory 
    //(texture_type, mipmaplevel, internal format, 
    // texSize, texSize, format, ..., no data now)
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, 
                 width, height, 0, formatFromString(format), GL_UNSIGNED_BYTE, 0);

    //set texture state vars
    textureInitialized_ = true;
    textureWidth_ = width;
    textureHeight_ = height;
    textureFormat_ = formatFromString(format);

}

void
ViewWidget::updateTexture()
{
    if(imageQueue_->isEmpty())
    {
        return;
    }
    
    orca::ImageDataPtr image;
    imageQueue_->getAndPop(image);
    orca::ImageDescriptionPtr descr = image->description;

    if( !textureInitialized_ 
        || textureWidth_ != static_cast<uint32_t>(descr->width)
        || textureHeight_ != static_cast<uint32_t>(descr->height)
        || textureFormat_ != formatFromString(descr->format)
      )
    {
        initializeTexture(descr->width, descr->height, descr->format);
    }

    // update the texture with the latest buffer
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, textureWidth_, textureHeight_,
                    formatFromString(descr->format), GL_UNSIGNED_BYTE, &(image->data[0]));
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
