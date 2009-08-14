/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLVERTEXSHADER_H
#define HYDRO_GLU_GLVERTEXSHADER_H

#include "glshader.h"

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace hydroglu
{

class GLVertexShader;

//! @brief Shared Pointer to a GLVertexShader
typedef IceUtil::Handle<GLVertexShader> GLVertexShaderPtr;

/**
 * @brief Wrapper for a GLSL Vertex Shader
 * Class that wraps a GLSL VertexShader and frequently used functionality
 */
class GLVertexShader : public GLShader, public virtual IceUtil::Shared
{
public:
    static GLVertexShaderPtr fromString(std::string source, std::string name);
    static GLVertexShaderPtr fromFile(std::string filename, std::string name);
    virtual ~GLVertexShader();

protected:
    GLVertexShader(std::string source, std::string name);
};

}

#endif
