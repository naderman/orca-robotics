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

#include <hydroutil/subsystemthread.h>
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
class MainThread : public hydroutil::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );
    ~MainThread();

    // from SubsystemThread
    virtual void walk();

private:

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until established
    void initNetworkInterface();

    void readData();

    // The laser object
    orcaifaceimpl::LaserScanner2dImpl *laserInterface_;

    hydrointerfaces::LaserScanner2d::Config config_;
    // an extra config to allow sensor mounted upside-down
    bool compensateRoll_;

    // space for data
    orca::LaserScanner2dDataPtr           orcaLaserData_;
    hydrointerfaces::LaserScanner2d::Data hydroLaserData_;

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
