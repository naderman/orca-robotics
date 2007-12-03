/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_MAIN_THREAD_H
#define ORCA2_LASER2D_MAIN_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>
// remote interface
#include <orcaifaceimpl/laserscanner2dImpl.h>
// hardware interface
#include <hydrointerfaces/laserscanner2d.h>

namespace laser2d {

//
// @brief the main executing loop of this laser component.
//
class MainThread : public hydroutil::SafeThread
{

public:

    MainThread( //orcaifaceimpl::LaserScanner2dImpl &laserInterface,
//               const hydrointerfaces::LaserScanner2d::Config &config,
//               hydrointerfaces::LaserScanner2dFactory &driverFactory,
//               bool                                compensateRoll,
              const orcaice::Context             &context );
    ~MainThread();

    // from SafeThread
    virtual void walk();

private:

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until activated
    void activate();
    // Loops until established
    void initNetworkInterface();

    void readData( orca::LaserScanner2dDataPtr &data );

    // The laser object
    orcaifaceimpl::LaserScanner2dImpl *laserInterface_;

    hydrointerfaces::LaserScanner2d::Config config_;

    // Generic driver for the hardware
//     hydrointerfaces::LaserScanner2d *driver_;

    // Loaded with this
//     hydrointerfaces::LaserScanner2dFactory &driverFactory_;

    bool compensateRoll_;

    // Generic driver for the hardware
    hydrointerfaces::LaserScanner2d *driver_;
    // A factory to instantiate the driver
    hydrointerfaces::LaserScanner2dFactory *driverFactory_;
    // And the library that provides it
    hydrodll::DynamicallyLoadedLibrary *driverLib_;

    orcaice::Context context_;
};

} // namespace

#endif
