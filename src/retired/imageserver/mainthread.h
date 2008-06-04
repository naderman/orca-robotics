/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/cameraImpl.h>

#include "driver.h"


namespace imageserver {

//
// @brief the main executing loop of this laser component.
//
// Note: this thing self-destructs when run() returns.
//
class MainThread : public gbxiceutilacfr::SafeThread
{

public:

    MainThread( orcaifaceimpl::CameraImpl&   cameraObj,
              Driver*                       hwDriver,
              const orcaice::Context&       context );

    ~MainThread();

    virtual void walk();

private:

    // The camera object
    orcaifaceimpl::CameraImpl& cameraObj_;

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
