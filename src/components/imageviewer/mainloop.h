/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGEVIEWER_MAINLOOP_H
#define ORCA2_IMAGEVIEWER_MAINLOOP_H

#include <highgui.h>
#include <cv.h>

#include <orca/camera.h>
#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>


namespace imageviewer{

class MainLoop : public orcaice::Thread
{
public:

    MainLoop( const orca::CameraConsumerPrx & callbackPrx,
                orcaice::Buffer<orca::CameraData> & dataPipe, 
                const orcaice::Context & context );

    ~MainLoop();

    virtual void run();

private:

    // Camera proxy
    orca::CameraPrx cameraPrx_;

    // local object to receive data
    orca::CameraConsumerPrx callbackPrx_;
    
    // buffers
    orcaice::Buffer<orca::CameraData> & dataPipe_;
    
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
