/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_IMAGESERVER_MAIN_LOOP_H
#define ORCA2_IMAGESERVER_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <orca/camera.h>
#include "cameraI.h"
#include "driver.h"
#include "imagegrabber/imagegrabber.h"


namespace imageserver {

//
// @brief the main executing loop of this laser component.
//
// Note: this thing self-destructs when run() returns.
//
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( CameraI              &cameraObj,
              Driver*              hwDriver,
              ImageGrabber*        imageGrabber,
              const orcaice::Context &context );

    ~MainLoop();

    virtual void run();

private:

    // The camera object
    CameraI &cameraObj_;

    // Generic driver for the hardware
    Driver *hwDriver_;

    ImageGrabber *imageGrabber_;

    orcaice::Context context_;

    // Loops until activated
    void activate();

    void readData( orca::CameraData& data );

};

} // namespace

#endif
