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
#include <orcaice/context.h>
#include <orcaifaceimpl/cameraI.h>

#include "driver.h"


namespace imageserver {

//
// @brief the main executing loop of this laser component.
//
// Note: this thing self-destructs when run() returns.
//
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( orcaifaceimpl::CameraI&   cameraObj,
              Driver*                   hwDriver,
              const orcaice::Context&   context );

    ~MainLoop();

    virtual void run();

private:

    // The camera object
    orcaifaceimpl::CameraI& cameraObj_;

    // Generic driver for the hardware
    Driver *hwDriver_;

    orcaice::Context context_;

    // Loops until activated
    void activate();
    // Loops until established
    void establishInterface();

    int readData( orca::CameraData& data );

};

} // namespace

#endif
