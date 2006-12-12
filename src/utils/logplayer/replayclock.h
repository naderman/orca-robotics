/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGPLAYER_REPLAY_CLOCK_H
#define ORCA2_LOGPLAYER_REPLAY_CLOCK_H

#include <IceUtil/Time.h>

namespace logplayer
{

class ReplayClock
{
public:
    // Throws an error string if replayRate is set to zero.
    ReplayClock( double replayRate=1.0 );

    void setLogStartTime( const IceUtil::Time & t ) { logStartTime_=t; };

    void setReplayStartTime( const IceUtil::Time & t ) { replayStartTime_=t; };

    IceUtil::Time untilLogTime( const IceUtil::Time & logDataTime );

private:
    double replayRate_;
    IceUtil::Time logStartTime_;
    IceUtil::Time replayStartTime_;
};

} // namespace

#endif
