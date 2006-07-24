/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGEVIEWER_CAMERA_CONSUMER_I_H
#define ORCA2_IMAGEVIEWER_CAMERA_CONSUMER_I_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orca/camera.h>

namespace imageviewer{

class CameraConsumerI : public orca::CameraConsumer
{
public:
    CameraConsumerI ( orcaice::PtrBuffer<orca::CameraDataPtr> &cameraDataBuffer );
    
    virtual void setData(const orca::CameraDataPtr& data, const Ice::Current&);

private:
    orcaice::PtrBuffer<orca::CameraDataPtr> &cameraDataBuffer_;
};

} // namespace
    
#endif

