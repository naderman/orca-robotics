/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orcaimagegrabber/imagegrabber.h>
// implementations of Ice objects
#include <orcaifaceimpl/cameraImpl.h>

namespace imageserver {

class Driver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:

//     MainThread* mainLoop_;
    gbxiceutilacfr::ThreadPtr mainThread_;

    //
    // HARDWARE INTERFACES
    //
    Driver *hwDriver_;
    orcaimage::ImageGrabber* imageGrabber_; 

    // Provided interface
    orcaifaceimpl::CameraImplPtr cameraInterface_;
};

}

#endif
