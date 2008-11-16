#ifndef SEGWAYRMP_ESTOP_INTERFACE_H
#define SEGWAYRMP_ESTOP_INTERFACE_H

#include <IceUtil/Mutex.h>
#include <orcaifaceimpl/estop.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace segwayrmp {

//
// @brief Establishes an orca::EStopInterface interface, and works out when 
//        the e-stop needs to be activated.
//
// Implementation: completes the generic EStopImpl by providing local functionality
// of AbstractEStopCallback.
//
// Note: we begin in the EStopInterface-triggered state: ie we're not moving unless
//       someone explicitly says it's OK.
//
// @author Alex Brooks
//
class EStopInterface : public orcaifaceimpl::AbstractEStopCallback
{

public: 

    EStopInterface( double keepAlivePeriodSec, const orcaice::Context &context );

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
