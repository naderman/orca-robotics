/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLTEXTURE_H
#define HYDRO_GLU_GLTEXTURE_H

#include "exceptions.h"

#include <string>

#include <GL/glew.h>
#include <GL/gl.h>

#include "glerrorutil.h"

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

#include <hydroimage/structures.h>

namespace hydroglu
{
class GLTexture;

/**
 * A Shared GLTexture Pointer
 * Keeps a reference count and is automatically destroyed when the count is 0
 */
typedef IceUtil::Handle<GLTexture> GLTexturePtr;

/**
 * @brief Class that wraps an OpenGL texture
 * Gives the ability to the user to use OpenGL textures in an object oriented manner
 */
class GLTexture : public virtual IceUtil::Shared
{
public:
    GLTexture(std::string name, uint32_t width, uint32_t height, GLenum type=GL_TEXTURE_2D, GLenum format=GL_RGBA, bool dma=true); 
    
    ~GLTexture();

    void upload( const uint8_t* const buffer, size_t bufferSize, GLenum bufferFormat );
    void download( const uint8_t* buffer , GLenum bufferFormat );

    void bind() const;
    void unbind() const;

    void pushBind();
    void popBind();

    std::string name() const;
    GLuint handle() const;
    uint32_t width() const;
    uint32_t height() const;
    GLenum type() const;
    GLenum format() const;
    bool normalized() const;

    void setMinFilter(GLenum filter);
    void setMagFilter(GLenum filter);
    void setWrapS(GLenum wrapS);
    void setWrapT(GLenum wrapT);

private:
    std::string name_;
    GLuint handle_;
    uint32_t width_;
    uint32_t height_;
    GLenum type_;
    GLenum format_;
    bool normalized_;
    bool dma_;
    GLuint pushedHandle_;
    GLuint buffer_;
};

#define TEXCOORD_BL(tex) 0.0, 0.0
#define TEXCOORD_TL(tex) 0.0, tex->normalized() ? 1.0 : tex->height()
#define TEXCOORD_BR(tex) tex->normalized() ? 1.0 : tex->width(), 0.0 
#define TEXCOORD_TR(tex) tex->normalized() ? 1.0 : tex->width(), tex->normalized() ? 1.0 : tex->height()


}

#endif

