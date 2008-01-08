/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
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
#include <hydroiceutil/safethread.h>
#include <hydroiceutil/buffer.h>
#include <orcaice/context.h>


namespace imageviewer{

class MainThread : public hydroiceutil::SafeThread
{
public:

    MainThread( const orca::CameraConsumerPrx & callbackPrx,
                hydroiceutil::Buffer<orca::CameraData> & dataPipe, 
                const orcaice::Context & context );

    ~MainThread();

    virtual void walk();

private:

    // Camera proxy
    orca::CameraPrx cameraPrx_;

    // local object to receive data
    orca::CameraConsumerPrx callbackPrx_;
    
    // buffers
    hydroiceutil::Buffer<orca::CameraData> & dataPipe_;
    
    orca::CameraDescription  descr_;

    orcaice::Context context_;

    IplImage* cvImage_;
    IplImage* cvImageTmp_;
    IplImage* bayerImage_;

    // setup opencv struct and window for display
    void initCvImage();
};

} // namespace

#endif
