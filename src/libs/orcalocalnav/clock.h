/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOCALNAV_CLOCK_H
#define ORCA_LOCALNAV_CLOCK_H

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <hydrotime/time.h>

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

    Clock( const hydrotime::Time &initialTime )
        { setTime( initialTime ); }

    void setTime( const hydrotime::Time &time )
        {
            timeStore_.set( time );
        }
    hydrotime::Time time() const
        {
            hydrotime::Time t;
            timeStore_.get( t );
            return t;
        }

private: 

    mutable gbxiceutilacfr::Store<hydrotime::Time> timeStore_;
};

}

#endif
