/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#include "glframebuffer.h"
#include <cstdio>

using namespace hydroglu;

/**
 * @brief GLFramebuffer constructor
 * @param name, used for debug output to easily identify which framebuffer the error comes from
 */
GLFrameBuffer::GLFrameBuffer(std::string name)
    : name_(name)
{
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxBuffers_);
    
    glGenFramebuffersEXT(1, &handle_);
    checkErrorsGL( ERROR_INFO );
      
}

GLFrameBuffer::~GLFrameBuffer()
{
}

/**
 * @brief attach a GLTexture to one of the OpenGL attachment points
 * @param hydroglu GLTexturePtr
 * @param enumerated attachment point, ex: GL_COLOR_ATTACHMENT0_EXT
 */
void 
GLFrameBuffer::attachTexture(GLTexturePtr texture, GLenum attachment)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, handle_); 

    //attach texture to fbo
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, texture->type(), texture->handle(), 0);
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    if( status != GL_FRAMEBUFFER_COMPLETE_EXT )
    {
        printf("there was an error attaching framebuffer\n");
    }
    
    checkErrorsGL( ERROR_INFO );
    
    printf("Attachment is %d\n", attachment); 
    
    textures_[attachment] = texture;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

/**
 * @brief obtain the texture attached to a point or an empty GLTexturePtr
 * @param enumerated attachment point, ex: GL_COLOR_ATTACHMENT0_EXT
 * @return a hydroglu GLTexturePtr that either points to the texture attached at the given attachment or is empty if there is none
 */
GLTexturePtr 
GLFrameBuffer::getTexture(GLenum attachment) const
{
    std::map<GLenum, GLTexturePtr>::const_iterator iter = textures_.find(attachment);
    if(iter->first != attachment && iter == textures_.end())
    {
        return GLTexturePtr();
    }
    
    return iter->second;
}

/**
 * @brief bind this framebuffer for usage. 
 * Binding a framebuffer redirects the output of drawing using opengl to be this framebuffer
 * same as glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, thisfbhandle );
 */
void 
GLFrameBuffer::bind() const
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, handle_);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief unbind this framebuffer
 * Unbinding a framebuffer means the render target is on screen again
 * same as glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
 */
void GLFrameBuffer::unbind() const
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    checkErrorsGL( ERROR_INFO );
}

