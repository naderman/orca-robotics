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

#include <orcaiceutil/thread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/laserscanner2dImpl.h>
#include <laser2dutil/driver.h>

namespace laser2d {

//
// @brief the main executing loop of this laser component.
//
// Note: this thing self-destructs when run() returns.
//
class MainLoop : public orcaiceutil::Thread
{

public:

    MainLoop( orcaifaceimpl::LaserScanner2dImpl &laserInterface,
              const Driver::Config               &config,
              DriverFactory                      &driverFactory,
              bool                                compensateRoll,
              const orcaice::Context             &context );
    ~MainLoop();

    virtual void run();

private:

    // Tries repeatedly to instantiate the driver
    void initialiseDriver();

    // Loops until activated
    void activate();
    // Loops until established
    void establishInterface();

    void readData( orca::LaserScanner2dDataPtr &data );

    // The laser object
    orcaifaceimpl::LaserScanner2dImpl &laserInterface_;

    Driver::Config config_;

    // Generic driver for the hardware
    Driver *driver_;

    // Loaded with this
    DriverFactory &driverFactory_;

    bool compensateRoll_;

    orcaice::Context context_;
};

}

#endif
