/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_TIME_UTILITIES_H
#define ORCAOBJ_TIME_UTILITIES_H

#include <orca/orca.h>

namespace orcaice
{

//! @name Time Operations
//@{

//! Returns current time.
orca::Time getNow();
//! Set to current time.
void setToNow( orca::Time & t );

//! Converts from orca::Time to Ice standard time format.
IceUtil::Time toIceTime( const orca::Time & t );
//! Converts from Ice standard time format to orca::Time.
orca::Time toOrcaTime( const IceUtil::Time & t );

//! Converts from number of seconds to orca::Time.
//! Time can be negative.
orca::Time toOrcaTime( double seconds );
//! Converts an orca::Time to a double in seconds.
//! Time can be negative, in which case both seconds and
//! microseconds are returned as negatives, 
//! e.g. -10.1s will translate to -10s:-100000us,
//! not -11s:900000us.
double timeAsDouble( const orca::Time &t );
//! Adds the specified number of seconds to t
void add( orca::Time &t, double seconds );
//! Subtracts the specified number of seconds to t
void subtract( orca::Time &t, double seconds );

//! Converts from Ice standard time format to orca::TimeOfDay.
//! Unlike IceUtil::Time::toDateTime(), we use UTC time and not local time.
orca::TimeOfDay toOrcaTimeOfDay( const IceUtil::Time & t );
//! Converts from Ice standard time format to orca::Date.
//! Unlike IceUtil::Time::toDateTime(), we use UTC time and not local time.
orca::Date toOrcaDate( const IceUtil::Time & t );

//! Returns the difference between two time values (ie: t1-t2)
//! The result is negative if t1 is earlier than t2.
orca::Time timeDiff( const orca::Time &t1, const orca::Time &t2 );
//! Returns the difference between two time values as a double (in seconds) (ie: t1-t2)
//! The result is negative if t1 is earlier than t2.
double timeDiffAsDouble( const orca::Time &t1, const orca::Time &t2 );

// NOTE: Time comparisons for orca::Time are done automatically by Ice.

//@}


} // namespace

#endif
