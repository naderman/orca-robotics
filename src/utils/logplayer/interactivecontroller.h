#ifndef LOGPLAYER_INTERACTIVECONTROLLER_H
#define LOGPLAYER_INTERACTIVECONTROLLER_H

#include "replayconductor.h"
#include <orcaice/context.h>
#include <hydroutil/safethread.h>

namespace logplayer {

//!
//! @author Alex Brooks
//!
class InteractiveController : public hydroutil::SafeThread
{

public: 

    InteractiveController( ReplayConductor        &replayConductor,
                           bool                    autoStart, 
                           const orcaice::Context &context );

    void walk();

private: 

    void parseInput( const std::string &input );
    void printMenu();

    ReplayConductor &replayConductor_;
    bool             autoStart_;

    orcaice::Context context_;

};

}

#endif
