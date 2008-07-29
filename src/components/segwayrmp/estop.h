#ifndef SEGWAYRMP_ESTOP_H
#define SEGWAYRMP_ESTOP_H

#include <IceUtil/Mutex.h>
#include <orcaifaceimpl/estopImpl.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace segwayrmp {

//
// @brief Establishes an orca::EStop interface, and works out when 
//        the e-stop needs to be activated.
//
// Note: we begin in the EStop-triggered state: ie we're not moving unless
//       someone explicitly says it's OK.
//
// @author Alex Brooks
//
class EStop : public orcaifaceimpl::EStopNonStandardImpl
{

public: 

    EStop( double                  keepAlivePeriodSec,
           const orcaice::Context &context );

    // Sets up the Ice interface
    void initInterface( gbxiceutilacfr::Thread* thread );

    // If true, we should _NOT_MOVE_ !!
    bool isEStopTriggered();

    // Inherited from orcaifaceimpl::EStopNonStandardImpl
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
