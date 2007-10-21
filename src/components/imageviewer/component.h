/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGEVIEWER_COMPONENT_H
#define ORCA2_IMAGEVIEWER_COMPONENT_H

#include <orcaice/component.h>
#include <hydroutil/buffer.h>
#include <orca/camera.h>


namespace imageviewer{

class MainLoop;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    hydroutil::Buffer<orca::CameraData> dataPipe_;
    MainLoop* mainloop_;
};
    
} // namespace

#endif
