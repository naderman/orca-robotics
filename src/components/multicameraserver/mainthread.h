/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>

// plugin interface
#include <hydrointerfaces/multiimage.h>

// remote interface
#include <orcaifaceimpl/multicamera.h>

namespace multicameraserver {

//
// @brief the main executing loop of this camera component.
//
class MainThread : public orcaice::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    // Setup Hardware
    void initHardwareInterface();

    // Loops until established
    void initNetworkInterface();

    // Read Data from the Driver
    void readData();

    // Read Settings from Config file
    void readSettings();

    // Context
    orcaice::Context context_;

    // The Network Image Interface object
    orcaifaceimpl::MultiCameraImplPtr interface_;

    // The Network Image Interface Data Structure
    orca::MultiCameraDataPtr orcaData_;

    // The Network Image Interface Description Structure
    orca::MultiCameraDescriptionPtr descr_;
    
    // The Driver Interface Config Structure
    hydrointerfaces::MultiImage::Config config_;

    // The Driver Interface Data Structure
    hydrointerfaces::MultiImage::Data hydroData_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::MultiImage> driver_;

};

} // namespace

#endif
