/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaiceutil/timer.h>

namespace orcaiceutil
{

Timer::Timer()   
{
    restart();
}

void Timer::restart()
{
    time_ = IceUtil::Time::now();
}

IceUtil::Time Timer::elapsed() const
{
    return IceUtil::Time::now() - time_;
}

double Timer::elapsedMs() const
{
    return ( (IceUtil::Time::now() - time_).toMilliSecondsDouble() );
}

double Timer::elapsedSec() const
{
    return ( (IceUtil::Time::now() - time_).toSecondsDouble() );
}

} // namespace
