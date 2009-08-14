/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLFRAMEBUFFER_H
#define HYDRO_GLU_GLFRAMEBUFFER_H



#include "exceptions.h"

#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "glerrorutil.h"
#include "gltexture.h"

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace hydroglu
{
class GLFrameBuffer;

/**
 * A Shared GLFrameBuffer Pointer
 * Keeps a reference count and is automatically destroyed when the count is 0
 */
typedef IceUtil::Handle<GLFrameBuffer> GLFrameBufferPtr;

/**
 * @brief Class that wraps an OpenGL framebuffer object in a simplistic manner
 * 
 * A framebuffer is a way of attaching what is normally a read only buffer such as a 
 * texture to something that can be written to (drawn on in the case of textures).
 *
 * Commonly used as the destination for GPGPU computations. 
 * 
 * Multiple textures can be attached as output destionations for a framebuffer such that
 * when the framebuffer is drawn to all of the attached textures are available in
 * a glsl shader. This way, multiple computations that need to be stored seperately
 * can be done in a single shader in a single pass.
 *
 * Reading from a GLFrameBuffer back to memory can be done through the GLTexture
 * download method.
 *
 * Typical usage
 * @verbatim
 #include <hydroglu/hydroglu.h>
 //... other code
 //
 void initializeGL()
 {
    //setup opengl state
    //...
    //end setup opengl state
    
    fftTexture_ = new GLTexture("FFT Output Buffer", 640, 480);
    frameBufferPtr_ = new GLFrameBuffer("FFT Output");
    frameBufferPtr_->attachTexture(fftTexture_, GL_COLOR_ATTACHMENT0_EXT);
 }

 void paintGL()
 {
    fftprogram_->enable();
    frameBufferPtr_->bind();
    //clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    //bind input texture
    fftinput_->bind();

    glBegin(GL_QUADS);

    //draw quad
    glEnd();

    fftinput_->unbind();
    frameBufferPtr_->unbind();
    fftprogram_->disable();

 }
@endverbatim
*/
class GLFrameBuffer : public virtual IceUtil::Shared
{
public:
    GLFrameBuffer(std::string name);
    ~GLFrameBuffer();

    void attachTexture(GLTexturePtr texture, GLenum attachment);
    GLTexturePtr getTexture(GLenum attachment) const;

    void bind() const;
    void unbind() const;

private:
    std::string name_;
    GLuint handle_;
    std::map<GLenum, GLTexturePtr> textures_;
    GLint maxBuffers_;
};

}

#endif
