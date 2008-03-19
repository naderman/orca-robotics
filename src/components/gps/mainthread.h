/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <gbxsickacfr/gbxiceutilacfr/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>
// remote interface
#include <orcaifaceimpl/gpsImpl.h>
// hardware interface
#include <hydrointerfaces/gps.h>

namespace gps {


class MainThread : public gbxsickacfr::gbxiceutilacfr::SubsystemThread
{

public:

    MainThread( const orcaice::Context& context );

    // from SubsystemThread
    virtual void walk();

private:

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until established
    void initNetworkInterface();

    orca::GpsDescription descr_;
    orcaifaceimpl::GpsImplPtr gpsInterface_;

    hydrointerfaces::Gps::Config config_;
    void reportBogusValues( orca::GpsData        &gpsData );
    
    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::Gps> driver_;

    orcaice::Context context_;
};

} // namespace

#endif
