/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_SUBSYSTEM_H
#define MAIN_SUBSYSTEM_H

#include <memory>
#include <orcaice/subsystem.h>
#include <hydrodll/dynamicload.h>
// remote interface
#include <orcaifaceimpl/gpsImpl.h>
// hardware interface
#include <hydrointerfaces/gps.h>

namespace gps {

class MainSubsystem : public orcaice::Subsystem
{

public:

    MainSubsystem( const orcaice::Context& context );

private:

    // from Subsystem
    virtual void initialise();
    virtual void work();

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until established
    void initNetworkInterface();

    orca::GpsDescription descr_;
    orcaifaceimpl::GpsImplPtr gpsInterface_;

    hydrointerfaces::Gps::Config config_;
    
    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::Gps> driver_;

    // options
    bool publishWithoutFix_;
};

} // namespace

#endif
