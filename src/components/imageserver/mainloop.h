/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER_MAIN_LOOP_H
#define ORCA2_LASER_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include "driver.h"
#include "imagegrabber/imagegrabber.h"
#include <orca/camera.h>
#include "cameraI.h"

#include <iostream>

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
              orcaice::Context     context,
              ImageGrabber*        imageGrabber,
              bool                 startEnabled, 
              std::string          driverName );

    ~MainLoop();

    virtual void run();

private:

    // The camera object
    CameraI &cameraObj_;

    // Generic driver for the hardware
    Driver *hwDriver_;

    orcaice::Context context_;

    ImageGrabber *imageGrabber_;

    std::string driverName_;
};

}

#endif
