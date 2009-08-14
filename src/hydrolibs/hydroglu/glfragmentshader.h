/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLFRAGMENTSHADER_H
#define HYDRO_GLU_GLFRAGMENTSHADER_H

#include "glshader.h"

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace hydroglu
{
    
class GLFragmentShader;
typedef IceUtil::Handle<GLFragmentShader> GLFragmentShaderPtr;

class GLFragmentShader : public GLShader, public virtual IceUtil::Shared
{
public:
    static GLFragmentShaderPtr fromString(std::string source, std::string name);
    static GLFragmentShaderPtr fromFile(std::string filename, std::string name);
    virtual ~GLFragmentShader();

protected:
    GLFragmentShader(std::string source, std::string name);
};

}

#endif
