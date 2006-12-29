/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include "heartbeater.h"
#include <orcaobj/timeutils.h>

using namespace std;

namespace orcaice {

Heartbeater::Heartbeater( const orcaice::Context & context, double secBetweenHeartbeats, double urgencyDivider )
    : secBetweenHeartbeats_(secBetweenHeartbeats),
      urgencyDivider_(urgencyDivider),
      context_(context)
{
    // First time we're queried, issue a heartbeat.
    lastHeartbeatTime_.seconds  = 0;
    lastHeartbeatTime_.useconds = 0;
}

bool
Heartbeater::isHeartbeatTime()
{
    double secSinceHeartbeat = orcaice::timeDiffAsDouble( orcaice::getNow(), lastHeartbeatTime_ );
    return ( secSinceHeartbeat >= secBetweenHeartbeats_ );
}

bool
Heartbeater::isHeartbeatTimeUrgent()
{
    double secSinceHeartbeat = orcaice::timeDiffAsDouble( orcaice::getNow(), lastHeartbeatTime_ );
    return ( secSinceHeartbeat >= (secBetweenHeartbeats_/urgencyDivider_) );
}

void
Heartbeater::beat( const std::string &heartbeatMessage )
{
    context_.tracer()->debug(heartbeatMessage);
    lastHeartbeatTime_ = orcaice::getNow();
}

}
