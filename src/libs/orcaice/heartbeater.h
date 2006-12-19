/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef HEARTBEATER_H
#define HEARTBEATER_H

#include <orca/orca.h>
#include <orcaice/context.h>
#include <string>

namespace orcaice {

//!
//! @brief Small utility class for keeping track of regular heartbeats.
//!
//! @author Alex Brooks
//!
class Heartbeater
{

public: 

    Heartbeater( const orcaice::Context & context, double secBetweenHeartbeats = 10.0, double urgencyDivider=4.0 );

    //! Is it time for the next heartbeat?
    bool isHeartbeatTime();

    //! Is it time for the next heartbeat (when there's some more urgent info to report)?
    bool isHeartbeatTimeUrgent();

    //! Doesn't check if it's time, just sends the heartbeat message
    void beat( const std::string &heartbeatMessage );

private: 

    orca::Time lastHeartbeatTime_;

    double secBetweenHeartbeats_;
    double urgencyDivider_;

    orcaice::Context context_;
};

}

#endif
