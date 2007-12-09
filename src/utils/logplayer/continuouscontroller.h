#ifndef LOGPLAYER_CONTINUOUSCONTROLLER_H
#define LOGPLAYER_CONTINUOUSCONTROLLER_H

#include <hydroutil/safethread.h>
#include "replayconductor.h"

namespace logplayer {

//
// @author Alex Brooks
//
class ContinuousController : public hydroutil::SafeThread
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
