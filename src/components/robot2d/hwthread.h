/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ROBOT2D_HARDWARE_THREAD_H
#define ROBOT2D_HARDWARE_THREAD_H

#include <hydroutil/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrointerfaces/robot2d.h>
#include <hydrodll/dynamicload.h>
#include <hydroutil/store.h>
#include <orcarobotdriverutil/statemachine.h>

namespace robot2d {

//
// @brief The thread that runs the driver
//
// @author Alex Brooks
//
class HwThread : public hydroutil::SubsystemThread
{

public: 

    HwThread( const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

    // local interface, used by NetThread

    void setCommand( const hydrointerfaces::Robot2d::Command &command );

    // Return valus same as hydroutil::Store.
    int getData( hydrointerfaces::Robot2d::Data &data, int timeoutMs )
        {
            return dataStore_.getNext( data, timeoutMs );
        }

private: 

    // Keeps trying until success or !!isStopping()
    void enableDriver();

    // Faults can be detected in either read or write threads: have to be careful.
    orcarobotdriverutil::StateMachine stateMachine_;

    // Stores the data most recently received from the hardware
    hydroutil::Store<hydrointerfaces::Robot2d::Data>    dataStore_;
    // Stores incoming commands
    hydroutil::Store<hydrointerfaces::Robot2d::Command> commandStore_;

    bool isMotionEnabled_;

    // Generic driver for the hardware
    hydrointerfaces::Robot2d *driver_;
    // A factory to instantiate the driver
    hydrointerfaces::Robot2dFactory *driverFactory_;
    // And the library that provides it
    hydrodll::DynamicallyLoadedLibrary *driverLib_;

    orcaice::Context context_;
};

} // namespace

#endif
