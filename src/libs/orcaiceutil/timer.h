/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_TIMER_H
#define ORCAICEUTIL_TIMER_H

#include <IceUtil/Time.h>

namespace orcaiceutil
{

/*!
 *  @brief A handy class that performs timing functions using Ice Time class.
 *
 *  Not thread-safe.
 *
 *  Call restart(), do something, then call elapsed() which returns the time
 *  elapsed since restart().
 *
 *  Can then use built-in functions to convert IceUtil::Time to timeval,
 *  double, millisec, etc.
 *
 *  Measures WallClockTime.
 */
class Timer
{
public:

    //! Sets internal clock to current time
    Timer();

    //! Resets internal clock to current time
    void restart();

    //! Returns time elapsed from last restart
    IceUtil::Time elapsed() const;

    //! returns elapsed milliseconds as a double
    double elapsedMs() const;

    //! returns elapsed seconds as a double
    double elapsedSec() const;

private:
    IceUtil::Time time_;

};

} // end namespace

#endif
