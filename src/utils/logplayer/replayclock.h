/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

//
// ReplayClock is used to make sure the log gets played with the correct timing
// (possibly not real-time).
//
// The timing could also be controlled by just looking at time deltas between items,
// but that would drift.  This provides some kind of absolute reference.
//
// We keep track of two things:
//  1. real time since the start of continuous playback, and
//  2. log time since the start of continuous playback.
//
class ReplayClock
{
public:
    // Throws an error string if replayRate is set to zero.
    ReplayClock( double replayRate=1.0 );

    void setReplayRate( double replayRate ) { replayRate_=replayRate; };

    // When we start continuous replay, make a note of the start time with this call
    void setContinuousReplayStartTime( const IceUtil::Time & t );

//    void setLogStartTime( const IceUtil::Time & t ) { logStartTime_=t; };
//    void setReplayStartTime( const IceUtil::Time & t ) { replayStartTime_=t; };

    // Give it the time in log-land till the next item, it returns the
    // time we should wait in the real world.
    // Note: setContinuousReplayStartTime should have been called already.
    IceUtil::Time realTimeTillNextItem( const IceUtil::Time &logTimeTillNextItem );

private:
    double replayRate_;
    IceUtil::Time logStartTime_;
    IceUtil::Time replayStartTime_;
};

} // namespace

#endif
