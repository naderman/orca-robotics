/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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

    logStartTime_ = IceUtil::Time::now();
    replayStartTime_ = IceUtil::Time::now();
}

IceUtil::Time 
ReplayClock::untilNextLogTime( const IceUtil::Time & logDataTime )
{
    IceUtil::Time replayDataTime;

    if ( replayRate_ == 1.0 ) {
        replayDataTime = replayStartTime_ + logDataTime - logStartTime_;
    }
    else {
        replayDataTime = replayStartTime_ + (logDataTime - logStartTime_)/replayRate_;
    }

    IceUtil::Time timeLeft = replayDataTime - IceUtil::Time::now();

    // debug
//     cout<<"log started           : "<<logStartTime_.toString()<<endl;
//     cout<<"replay started        : "<<replayStartTime_.toString()<<endl;
//     cout<<"next data log time    : "<<logDataTime.toString()<<endl;
//     cout<<"next data replay time : "<<replayDataTime.toString()<<endl;
//     cout<<"time remaining        : "<<timeLeft.toDuration()<<endl;

    return timeLeft;
}
