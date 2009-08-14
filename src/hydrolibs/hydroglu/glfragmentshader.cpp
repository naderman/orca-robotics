/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#include "glfragmentshader.h"
#include "exceptions.h"

using namespace hydroglu;

GLFragmentShader::GLFragmentShader(std::string source, std::string name)
: GLShader( GL_FRAGMENT_SHADER, source, name)
{
}

GLFragmentShader::~GLFragmentShader()
{
}

GLFragmentShaderPtr
GLFragmentShader::fromString(std::string source, std::string name)
{
    return GLFragmentShaderPtr(new GLFragmentShader(source, name));
}

GLFragmentShaderPtr 
GLFragmentShader::fromFile(std::string filename, std::string name)
{
    std::string source = GLShader::sourceFromFile(filename);
    return GLFragmentShaderPtr(new GLFragmentShader(source, name));
}
                
