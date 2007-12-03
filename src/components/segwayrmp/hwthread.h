/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SEGWAYRMP_HARDWARE_THREAD_H
#define SEGWAYRMP_HARDWARE_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydrointerfaces/segwayrmp.h>
#include <hydrodll/dynamicload.h>

#include <hydroutil/store.h>
#include <orcarobotdriverutil/statemachine.h>
#include <hydroutil/timer.h>

namespace segwayrmp {


enum EStopStatus{ESS_NO_FAULT, ESS_FAULT};

//
// @brief The thread that runs the driver
//
// @author Alex Brooks
//
class HwThread : public hydroutil::SafeThread
{

public: 

    // this is a subset of configuration parameters which is checked by the hardware driver
    struct Config
    {
        double maxForwardSpeed;
        double maxReverseSpeed;
        double maxTurnrate;
        double maxTurnrateAtMaxSpeed;
    };

    // these config settings are checked and possibly limited based on hardware capabilities
    HwThread( Config& config, const orcaice::Context &context );
    ~HwThread();
    
    // Inherited from SafeThread
    virtual void walk();

    // local interface, used by NetThread

    // used by NetThread to pass commands, events and get data
    void setCommand( const hydrointerfaces::SegwayRmp::Command &command );

    void setEStopFaultStatus( const EStopStatus status )
        {
            eStopFaultStatus_.set(status);
        }

    int getData( hydrointerfaces::SegwayRmp::Data &data, int timeoutMs )
        {
            return dataStore_.getNext( data, timeoutMs );
        }

private: 

    // Try and get data from the estop to ensure this is all OK.
    bool isEStopConnected(int timeoutMs);

    // Keeps trying until success or !!isStopping()
    void enableDriver();

    // Checks to see if the requested command is outside our limits.
    bool commandImpossible( const hydrointerfaces::SegwayRmp::Command &command );

    // Faults can be detected in either read or write threads: have to be careful.
    orcarobotdriverutil::StateMachine stateMachine_;

    // Stores the data most recently received from the hardware
    hydroutil::Store<hydrointerfaces::SegwayRmp::Data>    dataStore_;
    // Stores incoming commands
    hydroutil::Store<hydrointerfaces::SegwayRmp::Command> commandStore_;

    // Stores incoming EStop Status
    hydroutil::Store<EStopStatus> eStopFaultStatus_;

    double maxForwardSpeed_;
    double maxReverseSpeed_;
    double maxTurnrate_;

    bool isMotionEnabled_;
    bool isEStopEnabled_;

    // Looks for late writes (which will cause timeouts in the segway)
    hydroutil::Timer writeTimer_;

    // Generic driver for the hardware
    hydrointerfaces::SegwayRmp *driver_;
    // A factory to instantiate the driver
    hydrointerfaces::SegwayRmpFactory *driverFactory_;
    // And the library that provides it
    hydrodll::DynamicallyLoadedLibrary *driverLib_;

    orcaice::Context context_;
};

} // namespace

#endif
