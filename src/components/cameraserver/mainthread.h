/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include <orcaimagecommon/imagecomponentthread.h>
#include <hydrodll/dynamicload.h>

// remote interface
#include <orcaifaceimpl/camera.h>

namespace cameraserver {

//
// @brief the main executing loop of this camera component.
//
class MainThread : public orcaimagecommon::ImageComponentThread
{

public:

    MainThread( const orcaice::Context &context );

private:

    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    // Loops until established
    void initNetworkInterface();

    void readData();

    // The Network Image Interface object
    orcaifaceimpl::CameraImplPtr cameraInterface_;

    // The Network Image Interface Data Structure
    orca::CameraDataPtr orcaCameraData_;

    // The Network Image Interface Description Structure
    orca::CameraDescriptionPtr orcaCameraDescr_;

};

} // namespace

#endif
