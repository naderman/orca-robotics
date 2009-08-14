/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLSHADER_H
#define HYDRO_GLU_GLSHADER_H

#include "exceptions.h" 

#include <GL/glew.h>
#include <GL/gl.h>

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace hydroglu
{

class GLShader : public virtual IceUtil::Shared
{
public:
    GLuint handle() const;
    std::string source() const;
    std::string name() const;
    virtual ~GLShader();

protected:
    GLShader(GLenum type, std::string source, std::string name); 

    static std::string sourceFromFile(std::string filename);

private:
    void compile();
    
    GLenum type_;       //! shader type
    std::string name_;      //! shader name
    GLuint handle_;     //! shader handle
    std::string source_;    //! shader ascii source
};

typedef IceUtil::Handle<GLShader> GLShaderPtr;

}

#endif
