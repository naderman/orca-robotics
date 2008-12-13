/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft, Tom Burdick
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef DISPARITY_MAIN_THREAD_H
#define DISPARITY_MAIN_THREAD_H 

#include <memory>
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>

// plugin interface
#include <hydrointerfaces/disparity.h>

// network interfaces
#include <orcaifaceimpl/cameracollection.h>
#include <orcaifaceimpl/image.h>

namespace disparity {

class MainThread : public orcaice::SubsystemThread
{
public:
    MainThread( const orcaice::Context &context );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();
    virtual void finalise();

    // initialise the parameters for the Viewer
    void readSettings();

    // process data and read it back
    void processData();

    // Setup algorithm plugin
    void initPluginInterface();

    // Loops until established
    void initNetworkInterface();

    // context
    orcaice::Context context_;
    
    // incoming consumer object
    orcaifaceimpl::BufferedCameraCollectionConsumerImplPtr cameraCollectionInterface_;
    
    // incoming data structure
    orca::CameraCollectionDataPtr incomingData_;

    // The Network Image Interface object
    orcaifaceimpl::ImageImplPtr outgoingInterface_;

    // The Network Image Interface Data Structure
    orca::ImageDataPtr outgoingData_;

    // The Network Image Interface Description Structure
    orca::ImageDescriptionPtr outgoingDescr_;
    
    // The Driver Interface Config Structure
    hydrointerfaces::Disparity::Config pluginConfig_;

    // The Driver Interface Data Structure
    hydrointerfaces::Disparity::Data pluginData_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::Disparity> driver_;

};

} // namespace

#endif
