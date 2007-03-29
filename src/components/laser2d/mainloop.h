/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_MAIN_LOOP_H
#define ORCA2_LASER2D_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/laserscanner2diface.h>
#include "driver.h"

namespace laser2d {

//
// @brief the main executing loop of this laser component.
//
// Note: this thing self-destructs when run() returns.
//
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( orcaifaceimpl::LaserScanner2dIface &laserInterface,
              Driver                             *hwDriver,
              bool                                compensateRoll,
              const orcaice::Context             &context );
    ~MainLoop();

    virtual void run();

private:

    // Loops until activated
    void activate();
    // Loops until established
    void establishInterface();

    // Returns zero on succcess
    int readData( orca::LaserScanner2dDataPtr & data );

    // The laser object
    orcaifaceimpl::LaserScanner2dIface &laserInterface_;

    // Generic driver for the hardware
    Driver *hwDriver_;

    bool compensateRoll_;

    orcaice::Context context_;
};

}

#endif
