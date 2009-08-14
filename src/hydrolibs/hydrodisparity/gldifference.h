/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_STEREOCAMERA_GLDIFFERENCE_H
#define HYDRO_STEREOCAMERA_GLDIFFERENCE_H

#include <hydroglu/hydroglu.h>

namespace disparity 
{

class GLDifference : public hydroglu::GLProcess
{
public:
    GLDifference(size_t shifts, size_t offset);
    virtual ~GLDifference();

    virtual void initializeGL();
    virtual void processGL();
    virtual void setInputTextures( const std::vector<hydroglu::GLTexturePtr> & textures );
    virtual const std::vector<hydroglu::GLTexturePtr>& getOutputTextures();

private:
    std::vector<hydroglu::GLTexturePtr> inputTextures_;
    std::vector<hydroglu::GLTexturePtr> outputTextures_;
    std::vector<hydroglu::GLFrameBufferPtr> outputFramebuffers_;
    hydroglu::GLProgramPtr differenceProgram_;

    std::vector<GLfloat> leftVerts_;
    std::vector<GLfloat> rightVerts_;

    size_t shifts_;
    size_t offset_;
    size_t width_;
    size_t height_;

    bool initialized_;
    bool haveTextures_;
};

}
#endif
