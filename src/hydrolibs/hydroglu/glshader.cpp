/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#include "glerrorutil.h"
#include "glshader.h"
#include "exceptions.h"

#include <iostream>
#include <fstream>

using namespace hydroglu;

GLShader::GLShader(GLenum type, std::string source, std::string name)
: type_(type)
, name_(name)
, handle_(0)
, source_(source)
{
    assert( (type_ == GL_FRAGMENT_SHADER) || (type_ == GL_VERTEX_SHADER) );
    handle_ = glCreateShader(type_);
    if(handle_ == 0)
    {
        throw gbxutilacfr::Exception(ERROR_INFO, "failed to create glsl shader " + name_ );
    }
    compile();
}

GLShader::~GLShader()
{
    glDeleteShader(handle_);
}

GLuint
GLShader::handle() const
{
    return handle_;
}

std::string
GLShader::source() const
{
    return source_;
}
    
std::string
GLShader::name() const
{
    return name_;
}

void GLShader::compile()
{
    const char* sourcestr = source_.c_str();
    
    glShaderSource(handle_, 1, &sourcestr, NULL);

    glCompileShader(handle_);

    GLint status;
    glGetShaderiv(handle_, GL_COMPILE_STATUS, &status);
    if(!status) 
    {
        GLint blen = 0;
        GLsizei slen = 0;
        glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &blen);
        GLchar* log = new GLchar[blen];
        glGetShaderInfoLog(handle_, blen, &slen, log); 
        std::string logstr(log);
        delete[] log;
        throw CompileException(ERROR_INFO,  name_ + " glsl shader failed to compile, error log: \n" + logstr );
    } 
}
    
std::string 
GLShader::sourceFromFile(std::string filename)
{
    std::string source;
    
    std::ifstream stream( filename.c_str() );

    if(!stream.is_open())
    {
        throw FileReadException( ERROR_INFO, filename + " unable to open" );
    }
    
    while( !stream.eof() )
    {
        std::string line;
        std::getline( stream, line );
        source += line + '\n';
    }
    
    return source;
}

