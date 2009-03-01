/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "heartbeater.h"

// alexm: 
// This class is probably not needed anymore with the new Status.
// I quickly converted this class to use Timer instead of OrcaObj timing functions.
// haven't tested it so leaving it the original code to check later if necessary.


namespace orcaice {

Heartbeater::Heartbeater( const orcaice::Context & context, double secBetweenHeartbeats, double urgencyDivider )
    : secBetweenHeartbeats_(secBetweenHeartbeats),
      urgencyDivider_(urgencyDivider),
      tracer_(context.tracer())
{
    // First time we're queried, issue a heartbeat.
//     lastHeartbeatTime_.seconds  = 0;
//     lastHeartbeatTime_.useconds = 0;
}

bool
Heartbeater::isHeartbeatTime()
{
//     double secSinceHeartbeat = orcaice::timeDiffAsDouble( orcaice::getNow(), lastHeartbeatTime_ );
//     return ( secSinceHeartbeat >= secBetweenHeartbeats_ );
    return ( timer_.elapsedSec() >= secBetweenHeartbeats_ );
}

bool
Heartbeater::isHeartbeatTimeUrgent()
{
//     double secSinceHeartbeat = orcaice::timeDiffAsDouble( orcaice::getNow(), lastHeartbeatTime_ );
//     return ( secSinceHeartbeat >= (secBetweenHeartbeats_/urgencyDivider_) );
    return ( timer_.elapsedSec() >= (secBetweenHeartbeats_/urgencyDivider_) );
}

void
Heartbeater::beat( const std::string &heartbeatMessage )
{
    tracer_.debug(heartbeatMessage);
//     lastHeartbeatTime_ = orcaice::getNow();
    timer_.restart();
}

}
