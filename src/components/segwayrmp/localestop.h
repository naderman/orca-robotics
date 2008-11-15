#ifndef SEGWAYRMP_LOCAL_ESTOP_CALLBACK_H
#define SEGWAYRMP_LOCAL_ESTOP_CALLBACK_H

#include <IceUtil/Mutex.h>
#include <orcaifaceimpl/estop.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace segwayrmp {

//
// @brief Establishes an orca::LocalEstop interface, and works out when 
//        the e-stop needs to be activated.
//
// Note: we begin in the LocalEstop-triggered state: ie we're not moving unless
//       someone explicitly says it's OK.
//
// @author Alex Brooks
//
class LocalEstop : public orcaifaceimpl::AbstractEStopCallback
{

public: 

    LocalEstop( double keepAlivePeriodSec, const orcaice::Context &context );

    // Sets up the Ice interface
    void initInterface( gbxiceutilacfr::Thread* thread );

    // If true, we should _NOT_MOVE_ !!
    bool isEStopTriggered();

    // Inherited from orcaifaceimpl::AbstractEStopCallback
    void activateEStop();
    void keepAlive();
    double getRequiredKeepAlivePeriodSec();
    void setToOperatingMode();

private: 

    IceUtil::Mutex mutex_;

    bool                isEStopTriggered_;
    double              keepAlivePeriodSec_;
    gbxiceutilacfr::Timer keepAliveTimer_;

    orcaifaceimpl::EStopImplPtr eStopImpl_;

    orcaice::Context context_;

};

}

#endif
