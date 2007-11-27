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


enum EStopStatus{ESS_NO_FAULT, ESS_FAULT};

//
// @brief The thread that runs the driver
//
// @author Alex Brooks
//
class HwThread : public hydroutil::SafeThread
{

public: 

    HwThread( HwDriver               &hwDriver,
               double                  maxForwardSpeed,
               double                  maxReverseSpeed,
               double                  maxTurnrate,
               bool                    isMotionEnabled,
               bool                    isEStopEnabled,
               const orcaice::Context &context );

    // Inherited from SafeThread
    void walk();

    void setCommand( const Command &command );

    void setEStopFaultStatus( const EStopStatus status ){
        eStopFaultStatus_.set(status);
    }

    // Return valus same as hydroutil::Store.
    int getData( Data &data, int timeoutMs )
        {
            return dataStore_.getNext( data, timeoutMs );
        }

private: 

    // Try and get data from the estop to ensure this is all OK.
    bool isEStopConnected(int timeoutMs);

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

    // Stores incoming EStop Status
    hydroutil::Store<EStopStatus> eStopFaultStatus_;
    

    HwDriver &driver_;

    double maxForwardSpeed_;
    double maxReverseSpeed_;
    double maxTurnrate_;

    bool isMotionEnabled_;
    bool isEStopEnabled_;

    // Looks for late writes (which will cause timeouts in the segway)
    hydroutil::Timer writeTimer_;

    orcaice::Context context_;
};

}

#endif
