/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <highgui.h>
#include <cv.h>

#include <orca/camera.h>
#include <gbxsickacfr/gbxiceutilacfr/subsystemthread.h>
#include <orcaice/context.h>


namespace cameraviewer{

class MainThread : public gbxsickacfr::gbxiceutilacfr::SubsystemThread
{
public:

    MainThread( const orcaice::Context& context );
    ~MainThread();

    virtual void walk();

private:

    // Camera proxy
    orca::CameraPrx cameraPrx_;
    // local object to receive data
    orca::CameraConsumerPrx callbackPrx_;
    
    orca::CameraDescriptionSequence  descr_;

    std::vector<IplImage*> images_;
    
    // setup opencv struct and window for display
    void initCvImage();

    orcaice::Context context_;
};

} // namespace

#endif
