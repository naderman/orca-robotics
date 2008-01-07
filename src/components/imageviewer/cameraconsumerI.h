/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef CAMERA_CONSUMER_I_H
#define CAMERA_CONSUMER_I_H

#include <orcaice/component.h>
#include <hydroiceutil/buffer.h>
#include <orca/camera.h>

namespace imageviewer{

class CameraConsumerI : public orca::CameraConsumer
{
public:
    CameraConsumerI ( hydroiceutil::Buffer<orca::CameraData> &dataPipe );
    
    virtual void setData(const orca::CameraData& data, const Ice::Current&);

private:
    hydroiceutil::Buffer<orca::CameraData> &dataPipe_;
};

} // namespace
    
#endif

