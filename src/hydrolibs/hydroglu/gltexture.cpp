/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#include "gltexture.h"

#include <cstring>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace hydroglu;

GLTexture::GLTexture(std::string name, uint32_t width, uint32_t height, GLenum type, GLenum format, bool dma)
: name_(name)
, width_(width)
, height_(height)
, type_(type)
, format_(format)
, normalized_(true)
, dma_(dma)
{
    if( type_ != GL_TEXTURE_2D && type_ != GL_TEXTURE_RECTANGLE_ARB )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "bad texture type given for" ); 
    }

    if( type_ == GL_TEXTURE_RECTANGLE_ARB )
    {
        normalized_ = false;
    }

    if(dma_)
    {
        glGenBuffers(1, &buffer_);
        checkErrorsGL( ERROR_INFO );
    }

    glGenTextures(1, &handle_);
    checkErrorsGL( ERROR_INFO );
    glBindTexture( type_, handle_ );
    checkErrorsGL( ERROR_INFO );
    glTexImage2D( type_, 0, format, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
    checkErrorsGL( ERROR_INFO );
    
    setMagFilter(GL_LINEAR);
    setMinFilter(GL_LINEAR);
    setWrapT(GL_CLAMP);
    setWrapS(GL_CLAMP);
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &handle_);
}

void
GLTexture::upload( const uint8_t* const buffer, size_t bufferSize, GLenum bufferFormat)
{
    pushBind();

    if(dma_)
    {
        void* pData;
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, buffer_);
        glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, bufferSize, NULL, GL_STREAM_DRAW);
        pData = glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );
        std::memcpy( pData, buffer, bufferSize );
        glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );
        glTexSubImage2D( type_, 0, 0, 0, width_, height_, bufferFormat, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        checkErrorsGL( ERROR_INFO );
    }
    else
    {
        glTexSubImage2D(type_, 0, 0, 0, width_, height_, bufferFormat, GL_UNSIGNED_BYTE, buffer );
        checkErrorsGL( ERROR_INFO );
    }

    popBind();
}

void
GLTexture::download( const uint8_t* buffer, GLenum bufferFormat) 
{
    pushBind();
    glGetTexImage( type_, 0, bufferFormat, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(const_cast<uint8_t*>(buffer)) );
    checkErrorsGL( ERROR_INFO );
    popBind();
}

void 
GLTexture::bind() const
{
    glBindTexture( type_, handle_ );
    checkErrorsGL( ERROR_INFO );
}

void
GLTexture::unbind() const
{
    glBindTexture( type_, 0 );
    checkErrorsGL( ERROR_INFO );
}

std::string
GLTexture::name() const
{
    return name_;
}

GLuint
GLTexture::handle() const
{
    return handle_;
}

uint32_t
GLTexture::width() const
{
    return width_;
}

uint32_t
GLTexture::height() const
{
    return height_;
}

GLenum
GLTexture::type() const
{
    return type_;
}

GLenum
GLTexture::format() const
{
    return format_;
}

bool
GLTexture::normalized() const
{
    return normalized_;
}

void
GLTexture::pushBind()
{
    GLint val;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &(val));
    checkErrorsGL( ERROR_INFO );
    pushedHandle_ = static_cast<GLuint>(val);
    bind();
}

void
GLTexture::popBind()
{
    glBindTexture(GL_TEXTURE_2D, pushedHandle_);
    checkErrorsGL( ERROR_INFO );
}   

void
GLTexture::setMinFilter(GLenum filter)
{
    pushBind();
    glTexParameterf( type_, GL_TEXTURE_MIN_FILTER, filter );
    checkErrorsGL( ERROR_INFO );
    popBind();
}

void
GLTexture::setMagFilter(GLenum filter)
{
    pushBind();
    glTexParameterf( type_, GL_TEXTURE_MAG_FILTER, filter );
    checkErrorsGL( ERROR_INFO );
    popBind();
}

void
GLTexture::setWrapS(GLenum wrapS)
{
    pushBind();
    glTexParameterf( type_, GL_TEXTURE_WRAP_S, wrapS );
    checkErrorsGL( ERROR_INFO );
    popBind();
}

void
GLTexture::setWrapT(GLenum wrapT)
{
    pushBind();
    glTexParameterf( type_, GL_TEXTURE_WRAP_T, wrapT);
    checkErrorsGL( ERROR_INFO );
    popBind();
}

