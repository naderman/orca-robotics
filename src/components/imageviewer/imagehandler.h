/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGEVIEWER_IMAGE_HANDLER_H
#define ORCA2_IMAGEVIEWER_IMAGE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orca/camera.h>

#include <opencv/highgui.h>
#include <opencv/cv.h>


namespace imageviewer{

//class AlgorithmDriver;

class ImageHandler : public orcaice::Thread
{
public:

    ImageHandler( orca::CameraPrx cameraPrx,
                  orcaice::PtrBuffer<orca::CameraDataPtr> &cameraDataBuffer,
                  const orcaice::Context & context );

    ~ImageHandler();

    virtual void run();

private:
    
    // generic algorithm
    //AlgorithmDriver* driver_;

    // Camera proxy
    orca::CameraPrx cameraPrx_;
    
    // Camera config and geometry
    orca::CameraConfigPtr cameraConfigPtr_;
    orca::CameraGeometryPtr cameraGeometryPtr_;
    
    // buffers
    orcaice::PtrBuffer<orca::CameraDataPtr> &cameraDataBuffer_;
    
    orcaice::Context context_;

    IplImage* cvImage_;
    IplImage* bayerImage_;

    void init();
};

} // namespace

#endif
