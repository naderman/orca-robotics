/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOCALNAV_CLOCK_H
#define ORCA_LOCALNAV_CLOCK_H

#include <orcaiceutil/proxy.h>
#include <orca/orca.h>

namespace orcalocalnav {

//
// A thread-safe clock.
// This is used instead of the processor's clock because we might be running in simulation.
// So we read time of our incoming data rather than any global reference.
//
// @author Alex Brooks
//
class Clock
{

public: 

    Clock( const orca::Time &initialTime )
        { setTime( initialTime ); }

    void setTime( const orca::Time &time )
        {
            timeProxy_.set( time );
        }
    orca::Time time() const
        {
            orca::Time t;
            timeProxy_.get( t );
            return t;
        }

private: 

    mutable orcaiceutil::Proxy<orca::Time> timeProxy_;

};

}

#endif
