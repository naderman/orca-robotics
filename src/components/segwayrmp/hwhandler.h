#ifndef SEGWAYRMP_HWHANDLER_H
#define SEGWAYRMP_HWHANDLER_H

#include <hydroutil/safethread.h>
#include <hydroutil/store.h>
#include "types.h"
#include <orcarobotdriverutil/statemachine.h>
#include "hwdriver.h"
#include <orcaice/context.h>
#include <hydroutil/timer.h>

namespace segwayrmp {

//
// @brief The thread that runs the driver
//
// @author Alex Brooks
//
class HwHandler : public hydroutil::SafeThread
{

public: 

    HwHandler( HwDriver               &hwDriver,
               double                  maxForwardSpeed,
               double                  maxReverseSpeed,
               double                  maxTurnrate,
               bool                    isMotionEnabled,
               const orcaice::Context &context );

    // Inherited from SafeThread
    void walk();

    void setCommand( const Command &command );

    // Return valus same as hydroutil::Store.
    int getData( Data &data, int timeoutMs )
        {
            return dataStore_.getNext( data, timeoutMs );
        }

private: 

    // Keeps trying until success or !!isStopping()
    void enableDriver();

    // Checks to see if the requested command is outside our limits.
    bool commandImpossible( const Command &command );

    // Faults can be detected in either read or write threads: have to be careful.
    orcarobotdriverutil::StateMachine stateMachine_;

    // Stores the data most recently received from the hardware
    hydroutil::Store<Data>    dataStore_;
    // Stores incoming commands
    hydroutil::Store<Command> commandStore_;

    HwDriver &driver_;

    double maxForwardSpeed_;
    double maxReverseSpeed_;
    double maxTurnrate_;

    bool isMotionEnabled_;

    // Looks for late writes (which will cause timeouts in the segway)
    hydroutil::Timer writeTimer_;

    orcaice::Context context_;
};

}

#endif
