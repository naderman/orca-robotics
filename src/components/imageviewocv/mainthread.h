/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMAGEVIEW_MAIN_THREAD_H
#define IMAGEVIEW_MAIN_THREAD_H

#include "viewer.h"

#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/image.h>

namespace imageviewocv {

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
    void initSettings();

    // Loops until established
    void initNetworkInterface();

    // copies image to local member variable
    void getImage();
    
    orcaifaceimpl::BufferedImageConsumerImplPtr imageInterface_;
    
    // data structure
    orca::ImageDataPtr imageData_;

    orcaice::Context context_;

};

} // namespace

#endif
