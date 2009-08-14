/*
* Orca-Robotics Project: Components for robotics 
*               http://orca-robotics.sf.net/
* Copyright (c) 2008 Tom Burdick <thomas.burdick@gmail.com>
*
* This copy of Orca is licensed to you under the terms described in
* the LICENSE file included in this distribution.
*
*/

#ifndef HYDRO_GLU_GLPROCESS_H
#define HYDRO_GLU_GLPROCESS_H

#include "gltexture.h"

#include <vector>

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>


namespace hydroglu
{

/**
 * @brief An interface that describes an small portion of an GPGPU algorithm
 *
 * used for creating GPGPU algorithms and reusable OpenGL process objects.
 *
 * Should do as good of a job as possible of returning to the opengl state that was given 
 * at the end of processGL
 *
 * @author Tom Burdick <thomas.burdick@gmail.com>
 */
class GLProcess : public virtual IceUtil::Shared
{
public:
    virtual ~GLProcess() = 0;

    //! setup the opengl state and necessary variables, should not leave the state changed 
    virtual void initializeGL() = 0;

    //! run the gpgpu algorithm, do the actual work, should attempt to return the opengl state as it was given
    virtual void processGL() = 0;

    //! set the input textures, the data to work on
    inline void setInputTextures(const std::vector<GLTexturePtr>&);

    //! get the output textures, references to the results
    virtual const std::vector<GLTexturePtr>& getOutputTextures() = 0;
};

typedef IceUtil::Handle<GLProcess> GLProcessPtr;

}

#endif

