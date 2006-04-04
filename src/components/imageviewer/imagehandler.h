/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
