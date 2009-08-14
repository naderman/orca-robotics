/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#include "glvertexshader.h"
#include "exceptions.h"

using namespace hydroglu;

GLVertexShader::GLVertexShader(std::string source, std::string name)
: GLShader( GL_VERTEX_SHADER, source, name)
{
}

GLVertexShader::~GLVertexShader()
{
}

GLVertexShaderPtr 
GLVertexShader::fromString(std::string source, std::string name)
{
    IceUtil::Handle<GLVertexShader> tptr(new GLVertexShader(source, name));
    return tptr;
}

GLVertexShaderPtr 
GLVertexShader::fromFile(std::string filename, std::string name)
{
    std::string source = GLShader::sourceFromFile(filename);
    GLVertexShaderPtr tptr(new GLVertexShader(source, name));
    return tptr;
}
