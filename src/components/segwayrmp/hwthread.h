/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SEGWAYRMP_HARDWARE_THREAD_H
#define SEGWAYRMP_HARDWARE_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrointerfaces/segwayrmp.h>
#include <hydrodll/dynamicload.h>

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcarobotdriverutil/statemachine.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include "estopinterface.h"
#include "setpoint.h"

namespace segwayrmp {


//
// @brief The thread that runs the driver
//
// @author Alex Brooks
//
class HwThread : public orcaice::SubsystemThread
{

public: 

    // this is a subset of configuration parameters which is checked by the hardware driver
    struct Config
    {
        double maxForwardSpeed;
        double maxReverseSpeed;
        double maxTurnrate;
        double maxForwardAcceleration;
        double maxReverseAcceleration;
        double maxLateralAcceleration;
    };

    // these config settings are checked and possibly limited based on hardware capabilities
    HwThread( Config& config, const orcaice::Context &context );

    // local interface, used by NetThread

    // used by NetThread to pass commands, events and get data
    void setCommand( const hydrointerfaces::SegwayRmp::Command &command );

    int getData( hydrointerfaces::SegwayRmp::Data &data, int timeoutMs )
        {
            return dataStore_.getNext( data, timeoutMs );
        }

private: 

    // Inherited from SubsystemThread
    virtual void walk();

    // Keeps trying until success or isStopping()
    void enableDriver();
    void writeCommand( hydrointerfaces::SegwayRmp::Command &command );

    // Checks with the driver, and sets the stateMachine_ accordingly.
    void checkStatus();

    // Faults can be detected in either read or write threads: have to be careful.
    orcarobotdriverutil::StateMachine stateMachine_;

    // Stores the data most recently received from the hardware
    gbxiceutilacfr::Store<hydrointerfaces::SegwayRmp::Data>    dataStore_;
    // Stores incoming commands
    gbxiceutilacfr::Store<hydrointerfaces::SegwayRmp::Command> commandStore_;

    bool isMotionEnabled_;
    bool driveInReverse_;

    std::auto_ptr<EStopInterface> estopInterface_;
    std::auto_ptr<SetPoint> speedSetPoint_;

    // Looks for late writes (which will cause timeouts in the segway)
    gbxiceutilacfr::Timer writeTimer_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::SegwayRmp> driver_;

    orcaice::Context context_;
};

} // namespace

#endif
