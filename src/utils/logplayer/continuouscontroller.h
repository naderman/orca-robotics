#ifndef LOGPLAYER_CONTINUOUSCONTROLLER_H
#define LOGPLAYER_CONTINUOUSCONTROLLER_H

#include <orcaice/subsystemthread.h>
#include "replayconductor.h"

namespace logplayer {

//
// @author Alex Brooks
//
class ContinuousController : public orcaice::SubsystemThread
{

public: 

    ContinuousController( ReplayConductor &replayConductor,
                          bool             autoStart,
                          const orcaice::Context &context );

    void walk();

private: 

    ReplayConductor &replayConductor_;
    bool             autoStart_;

    orcaice::Context context_;
};

}

#endif
