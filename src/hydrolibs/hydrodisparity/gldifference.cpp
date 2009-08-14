/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#include "gldifference.h"

#include <sstream>

using namespace disparity;

const char vertSource[]=
"void main()\n"
"{\n"
"   gl_Position = ftransform();\n"
"}\n"
"\n";

const char fragSource[]=
"uniform sampler2D left;\n"
"uniform sampler2D right;\n"
"void main ()\n"
"{\n"
"        gl_FragColor = abs(left - right);\n"
"}\n"
"\n";


GLDifference::GLDifference( size_t shifts, size_t offset )
  : shifts_(shifts)
  , offset_(offset)
  , initialized_(false)
  , haveTextures_(false)
{
    //sanity checks
    if( shifts == 0 )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "0 shifts given" );
    }
}

GLDifference::~GLDifference()
{
}

void 
GLDifference::initializeGL()
{
    //push all opengl state attributes
    glPushAttrib(GL_ALL_ATTRIB_BITS);    
    
    if(initialized_)
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "already initialized" );
    }
    
    //create difference program
    hydroglu::GLVertexShaderPtr vertShader = 
        hydroglu::GLVertexShader::fromString(vertSource, "Vertex");
    hydroglu::GLFragmentShaderPtr fragShader =
        hydroglu::GLFragmentShader::fromString(fragSource, "Fragment");
    
    differenceProgram_ = new hydroglu::GLProgram("Difference");
    differenceProgram_->addShader(vertShader);
    differenceProgram_->addShader(fragShader);
    differenceProgram_->link();

    initialized_ = true;

    //pop all opengl state attributes
    glPopAttrib();
}

void 
GLDifference::setInputTextures( const std::vector<hydroglu::GLTexturePtr> & textures )
{
    //sanity checks
    if(!initialized_)
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "no OpenGL context available");
    }

    if(textures.size() != 2)
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "input texture vector size is not 2");
    }

    if(textures.at(1)->width() != textures.at(0)->width())
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "input texture widths do not match");
    }

    if(textures.at(1)->height() != textures.at(0)->height())
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "input texture heights do not match");
    }

    if(textures.at(1)->type() != textures.at(0)->type())
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "input texture types do not match");
    }

    if(textures.at(1)->format() != textures.at(0)->format())
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "input texture formats do not match");
    }

    inputTextures_ = textures;

    width_ = textures.at(0)->width();
    height_ = textures.at(0)->height();

    //clear output textures and framebuffers
    outputTextures_.clear();
    outputFramebuffers_.clear();

    //create output textures and attach them to framebuffer objects
    for(size_t i = 0; i < shifts_; ++i)
    {
        std::stringstream namess;
        namess << "Difference for Shift " << i;
        std::string name(namess.str());
       
        // create the actual outputTextures
        outputTextures_.push_back( new hydroglu::GLTexture( name.c_str()
                                                   , textures[0]->width()
                                                   , textures[0]->height()
                                                   , textures[0]->type()
                                                )
                                 );
    
        //create framebuffer objects and attach the corresponding texture
        outputFramebuffers_.push_back( hydroglu::GLFrameBufferPtr( new hydroglu::GLFrameBuffer( name.c_str() ) ) );
        outputFramebuffers_[i]->attachTexture( outputTextures_[i], GL_COLOR_ATTACHMENT0_EXT );
    }
   
    //create vertex buffers
    


    //update output processes that have been chained to this one so that they have the updated buffers
    
    
    haveTextures_ = true;
}

const std::vector<hydroglu::GLTexturePtr>& 
GLDifference::getOutputTextures()
{
    if(!haveTextures_)
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "no output textures available");
    }

    return outputTextures_;
}


void 
GLDifference::processGL()
{
    //push all attributes on to stack
    glPushAttrib(GL_ALL_ATTRIB_BITS);


    //setup opengl state
    glClearDepth(1.0);
    glClearColor(0.3, 0.0, 0.3, 0.0);
    glShadeModel(GL_FLAT);
 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    
    //setup an ortho2d view correctly
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width_, height_);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 0.0, width_, height_);

    //enable the difference program
    differenceProgram_->enable();
    
    //iterate over the shifts
    for(uint32_t i = 0; i < shifts_; ++i)
    {
        //bind the correct framebuffer object
        
        //update the vertices
        
        //draw the two textures shifted appropriatly
           
        //disable the framebuffer for the particular shift
    }
    //disable the difference program
    

    //wait for all commands to go through
    glFinish();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    //pop all attributes back to the callers state
    glPopAttrib();

    //iterate over attached output processes and call them
}

