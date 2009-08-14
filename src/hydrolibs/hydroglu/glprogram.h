/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLPROGRAM_H
#define HYDRO_GLU_GLPROGRAM_H

#include "glshader.h"
#include "exceptions.h"

#include <string>
#include <vector>

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace hydroglu
{

/**
 * @brief Wrapper for a GLSL program
 * This class wraps most functionality needed to use GLSL, a compact language that typically is run in a vector processing manner
 * on a video card.
 */
class GLProgram : public virtual IceUtil::Shared
{   
public:
    GLProgram(std::string name);
    ~GLProgram();

    GLuint handle() const;

    void addShader(GLShaderPtr);
    void removeShader(GLShaderPtr);
    void link();

    void enable();
    void disable();

    //1 var
    void setUniform(std::string uniformName, int value0);
    void setUniform(std::string uniformName, float value0);
    //2 vars
    void setUniform(std::string uniformName, int value0, int value1);
    void setUniform(std::string uniformName, float value0, float value1);
    //3 vars
    void setUniform(std::string uniformName, int value0, int value1, int value2);
    void setUniform(std::string uniformName, float value0, float value1, float value2);
    //4 vars
    void setUniform(std::string uniformName, int value0, int value1, int value2, int value3);
    void setUniform(std::string uniformName, float value0, float value1, float value2, float value3);

private:
    GLint getUniformLocation(std::string uniformName);
    std::vector<GLShaderPtr> shaders_;
    GLuint handle_;
    std::string name_;
};

typedef IceUtil::Handle<GLProgram> GLProgramPtr;

} // namespace

#endif
