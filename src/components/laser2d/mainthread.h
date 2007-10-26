/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_MAIN_THREAD_H
#define ORCA2_LASER2D_MAIN_THREAD_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/laserscanner2dImpl.h>
#include <laser2dutil/driver.h>

namespace laser2d {

//
// @brief the main executing loop of this laser component.
//
class MainThread : public hydroutil::Thread
{

public:

    MainThread( orcaifaceimpl::LaserScanner2dImpl &laserInterface,
              const Driver::Config               &config,
              DriverFactory                      &driverFactory,
              bool                                compensateRoll,
              const orcaice::Context             &context );
    ~MainThread();

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
