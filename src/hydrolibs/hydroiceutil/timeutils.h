/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_TIME_UTILITIES_H
#define HYDROICEUTIL_TIME_UTILITIES_H

#include <IceUtil/Time.h>

namespace hydroiceutil
{

/*!
 *  @name Time Utilities
 */
//@{

//! Current time in seconds and microseconds.
void now( int& seconds, int& useconds );

//! Converts time in IceUtil format to seconds and microseconds.
void timeFromIceUtil( const IceUtil::Time& time, int& seconds, int& useconds );

//! Returns a string with time duration in the following format (with default separator):
//! "D:H:M:S.sss"
//! Numeric values are not padded, i.e. 5min appears as 5 (not 05).
//! Separator string can be specified. 
//!
//! E.g. "0 0 58 12.232"
std::string toDuration( const IceUtil::Time& time, const std::string& separator=":" );

//! Converts from number of seconds to seconds and microseconds.
//! Time can be negative, in which case both seconds and
//! microseconds are returned as negatives,
//! e.g. -10.1s will translate to -10s:-100000us,
//! not -11s:900000us.
void timeFromDouble( double time, int& seconds, int& useconds );

//! Convenience function, works as the one above.
IceUtil::Time timeFromDouble( double time );

//! Converts time in seconds and microseconds to a double in seconds.
//! Time can be negative, but remember that seconds and
//! microseconds are added to together.
//! e.g. -10s:-100000us and -11s:900000us will both translate to -10.1s
double timeAsDouble( int seconds, int useconds );

//@}
} // namespace

#endif
