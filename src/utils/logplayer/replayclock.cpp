/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "replayclock.h"

using namespace std;
using namespace logplayer;


ReplayClock::ReplayClock( double replayRate )
    : replayRate_(replayRate)
{
    if ( replayRate_==0 ) {
        throw std::string("Replay rate cannot be zero");
    }
}

void 
ReplayClock::setReplayRate( double replayRate )
{ 
    replayRate_=replayRate;
}

void
ReplayClock::setContinuousReplayStartTime( const IceUtil::Time &timeFirstLogItem )
{
    logStartTime_ = timeFirstLogItem;
    replayStartTime_ = IceUtil::Time::now();
}

IceUtil::Time 
ReplayClock::realTimeTillNextItem( const IceUtil::Time &logTimeTillNextItem )
{
    IceUtil::Time replayDataTime;

    if ( replayRate_ == 1.0 ) {
        replayDataTime = replayStartTime_ + logTimeTillNextItem - logStartTime_;
    }
    else {
        replayDataTime = replayStartTime_ + (logTimeTillNextItem - logStartTime_)/replayRate_;
    }

    IceUtil::Time timeLeft = replayDataTime - IceUtil::Time::now();

    // debug
//     cout<<"log started           : "<<logStartTime_.toString()<<endl;
//     cout<<"replay started        : "<<replayStartTime_.toString()<<endl;
//     cout<<"next data log time    : "<<logTimeTillNextItem.toString()<<endl;
//     cout<<"next data replay time : "<<replayDataTime.toString()<<endl;
//     cout<<"time remaining        : "<<timeLeft.toDuration()<<endl;

    return timeLeft;
}
