/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMAGEVIEW_MAIN_SUBSYSTEM_H
#define IMAGEVIEW_MAIN_SUBSYSTEM_H

#include "viewer.h"

#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/image.h>
// #include <orca/image.h>

// #include <hydrodll/dynamicload.h>

namespace imageview {

class MainSubsystem : public orcaice::SubsystemThread
{
public:
    MainSubsystem( const orcaice::Context &context );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();
    virtual void finalise();

    // initialise the parameters for the Viewer
    void initSettings();

//     Instantiate the viewer
    void createViewer();

    // Loops until established
    void initNetworkInterface();

    // copies image to local queue which the Viewer can access.
    void getImage();
    
    Viewer* viewer_;

    orcaifaceimpl::BufferedImageConsumerImplPtr imageInterface_;
    
    // data structures
    orca::ImageDataPtr imageData_;

    // local buffer for storing images received from communication buffers.
    // gbxiceutilacfr::Buffer<orca::ImageDataPtr> imageQueue_;

    orcaice::Context context_;

//     // The laser object
//     orcaifaceimpl::LaserScanner2dImplPtr laserInterface_;
// 
//     hydrointerfaces::LaserScanner2d::Config config_;
//     // an extra config to allow sensor mounted upside-down
//     bool compensateRoll_;
// 
//     // space for data
//     orca::LaserScanner2dDataPtr           orcaLaserData_;
//     hydrointerfaces::LaserScanner2d::Data hydroLaserData_;
// 
//     // The library that contains the driver factory (must be declared first so it's destructed last!!!)
//     std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
//     // Generic driver for the hardware
//     std::auto_ptr<hydrointerfaces::LaserScanner2d> driver_;
    
};

} // namespace

#endif
