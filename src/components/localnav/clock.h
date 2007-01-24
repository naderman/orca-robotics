#ifndef LOCALNAV_CLOCK_H
#define LOCALNAV_CLOCK_H

#include <orcaice/proxy.h>
#include <orca/orca.h>

namespace localnav {

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

    mutable orcaice::Proxy<orca::Time> timeProxy_;

};

}

#endif
