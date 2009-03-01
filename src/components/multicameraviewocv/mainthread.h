/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef CAMERACOLLECTIONVIEW_OCV_MAIN_THREAD_H
#define CAMERACOLLECTIONVIEW_OCV_MAIN_THREAD_H

#include "viewer.h"

#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/multicamera.h>

namespace multicameraviewocv {

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
    void getImages();
    
    orcaifaceimpl::BufferedMultiCameraConsumerImplPtr multiCameraInterface_;
    
    // data structure
    orca::MultiCameraDataPtr multiCameraData_;

    orcaice::Context context_;

};

} // namespace

#endif
