/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_COMMON_IMAGE_COMPONENT_THREAD_H
#define ORCA_IMAGE_COMMON_IMAGE_COMPONENT_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>

// hardware interface
#include <hydrointerfaces/image.h>

namespace orcaimagecommon {

//
// @brief A Small but Common MainThread class for Image based components 
//
class ImageComponentThread : public orcaice::SubsystemThread
{
public:
    ImageComponentThread( const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk() {};


protected:
    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    //! Read settings from a config file
    void readSettings();

    orcaice::Context context_;

    // The Driver Interface Config Structure
    hydrointerfaces::Image::Config config_;

    // The Driver Interface Data Structure
    hydrointerfaces::Image::Data      hydroImageData_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::Image> driver_;
    
};

} // namespace

#endif

