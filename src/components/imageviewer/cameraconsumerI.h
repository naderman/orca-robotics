/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGEVIEWER_CAMERA_CONSUMER_I_H
#define ORCA2_IMAGEVIEWER_CAMERA_CONSUMER_I_H

#include <orcaice/component.h>
#include <orcaiceutil/buffer.h>
#include <orca/camera.h>

namespace imageviewer{

class CameraConsumerI : public orca::CameraConsumer
{
public:
    CameraConsumerI ( orcaiceutil::Buffer<orca::CameraData> &dataPipe );
    
    virtual void setData(const orca::CameraData& data, const Ice::Current&);

private:
    orcaiceutil::Buffer<orca::CameraData> &dataPipe_;
};

} // namespace
    
#endif

