/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef REPLAY_CLOCK_H
#define REPLAY_CLOCK_H

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

    void setReplayRate( double replayRate );

    // When we start continuous replay, use this call to tell the clock the
    // time (in log-space) of the first item.
    void setContinuousReplayStartTime( const IceUtil::Time &timeFirstLogItem );

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
