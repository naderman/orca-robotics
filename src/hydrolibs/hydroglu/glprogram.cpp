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
#include "glprogram.h"
#include "glfragmentshader.h"
#include "glvertexshader.h"
#include "exceptions.h"

using namespace hydroglu;

/**
 * @brief GLProgram constructor
 * @param a name for the program used for debug output to help identify which GLProgram has a problem
 */
GLProgram::GLProgram(std::string name)
: handle_(0)
, name_(name)
{
    handle_ = glCreateProgram();
    if(handle_ == 0)
    {
        throw gbxutilacfr::Exception(ERROR_INFO, std::string("failed to create glsl program " + name_));
    }
}

/**
 * @brief GLProgram destructor
 */
GLProgram::~GLProgram()
{
    for(unsigned int i = 0; i < shaders_.size(); i++) 
    {
        glDetachShader(handle_, shaders_.at(i)->handle());
    }
    
    glDeleteProgram(handle_);
}

/**
 * @brief add a shader to the program
 * @param a hydroglu GLShaderPtr, which can either be a Fragment or Vertex shader.
 */
void
GLProgram::addShader(GLShaderPtr shader)
{
    assert(shader);
    shaders_.push_back(shader);
}

/**
 * @brief remove a shader from the program
 */
void
GLProgram::removeShader(GLShaderPtr shader)
{
    for(unsigned int i = 0; i < shaders_.size(); i++) 
    {
        if(shader->handle() == shaders_.at(i)->handle()) 
        {
            glDetachShader(handle_, shaders_.at(i)->handle());
        }
    }
}

/**
 * @brief link the glsl program from a set of compiled shaders
 * Much like C++ linking takes a set of object files and builds a complete program from them
 * so too does glsl.
 */
void
GLProgram::link()
{
    if(shaders_.size() == 0)
    {
        throw LinkException(ERROR_INFO, "no shaders given");
    }
    
    for(unsigned int i = 0; i < shaders_.size(); i++) 
    {
        glAttachShader(handle_, shaders_.at(i)->handle());
    }

    glLinkProgram(handle_);
    
    GLint linked;
    glGetProgramiv(handle_, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint blen = 0;
        GLsizei slen = 0;
        glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &blen);
        GLchar* log = new GLchar[blen];
        glGetProgramInfoLog(handle_, blen, &slen, log); 
        std::string logstr(log);
        delete[] log;
        throw LinkException(ERROR_INFO, std::string( name_ + " glsl program failed to link, error log: \n" + logstr ));
    }

}

/**
 * @brief returns the program handle
 * @return OpenGL glsl program handle
 */
GLuint 
GLProgram::handle() const
{
    return handle_;
}

/**
 * @brief enable the glsl program
 * enabling a glsl program means rendering will now use your glsl program instead of the fixed pipeline rendering
 */
void
GLProgram::enable()
{
    glUseProgram(handle_);
}

/**
 * @brief disable the glsl program
 * disabling a glsl program completely disables the usage glsl programs and goes back to fixed pipeline renering
 */
void
GLProgram::disable()
{
    glUseProgram(0);
}

/**
 * @brief get the integer location of a uniform given its name
 * @param uniform name
 * @return location of uniform
 */
GLint
GLProgram::getUniformLocation(std::string uniformName)
{
    const char* name = uniformName.c_str();
    
    int location = glGetUniformLocation(handle_, name);

    if(location == -1)
    {
        throw UniformNotFoundException( ERROR_INFO, std::string( "uniform variable " + uniformName + " in shader " + name_ + " not found" ));
    }

    return location;
}

/**
 * @brief set a uniform using 1 integer values
 * @param uniform name
 * @param value0
 */
void
GLProgram::setUniform(std::string uniformName, int value0)
{
    int location = getUniformLocation(uniformName);
    glUniform1i(location, value0);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 1 float values
 * @param uniform name
 * @param value0
 */
void
GLProgram::setUniform(std::string uniformName, float value0)
{
    int location = getUniformLocation(uniformName);
    glUniform1f(location, value0);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 2 integer values
 * @param uniform name
 * @param value0
 * @param value1
 */
void
GLProgram::setUniform(std::string uniformName, int value0, int value1)
{
    int location = getUniformLocation(uniformName);
    glUniform2i(location, value0, value1);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 2 float values
 * @param uniform name
 * @param value0
 * @param value1
 */
void
GLProgram::setUniform(std::string uniformName, float value0, float value1)
{
    int location = getUniformLocation(uniformName);
    glUniform2f(location, value0, value1);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 3 integer values
 * @param uniform name
 * @param value0
 * @param value1
 * @param value2
 */
void
GLProgram::setUniform(std::string uniformName, int value0, int value1, int value2)
{
    int location = getUniformLocation(uniformName);
    glUniform3i(location, value0, value1, value2);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 3 float values
 * @param uniform name
 * @param value0
 * @param value1
 * @param value2
 */
void
GLProgram::setUniform(std::string uniformName, float value0, float value1, float value2)
{
    int location = getUniformLocation(uniformName);
    glUniform3f(location, value0, value1, value2);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 4 integer values
 * @param uniform name
 * @param value0
 * @param value1
 * @param value2
 * @param value3
 */
void
GLProgram::setUniform(std::string uniformName, int value0, int value1, int value2, int value3)
{
    int location = getUniformLocation(uniformName);
    glUniform4i(location, value0, value1, value2, value3);
    checkErrorsGL( ERROR_INFO );
}

/**
 * @brief set a uniform using 4 float values
 * @param uniform name
 * @param value0
 * @param value1
 * @param value2
 * @param value3
 */
void
GLProgram::setUniform(std::string uniformName, float value0, float value1, float value2, float value3)
{
    float location = getUniformLocation(uniformName);
    glUniform4f(location, value0, value1, value2, value3);
    checkErrorsGL( ERROR_INFO );
}
