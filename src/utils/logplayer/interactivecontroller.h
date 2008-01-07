#ifndef LOGPLAYER_INTERACTIVECONTROLLER_H
#define LOGPLAYER_INTERACTIVECONTROLLER_H

#include "replayconductor.h"
#include <orcaice/context.h>
#include <hydroiceutil/subsystemthread.h>

namespace logplayer {

//!
//! @author Alex Brooks
//!
class InteractiveController : public hydroiceutil::SubsystemThread
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
