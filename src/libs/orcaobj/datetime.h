/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_DATETIME_H
#define ORCAOBJ_DATETIME_H

#include <Ice/Properties.h>
#include <orca/datetime.h>

namespace orcaobj
{

//! For durations less than 24hrs returns string HH:MM:SS.sss.
//! Otherwise, DD:HH:MM:SS.sss
//! Negative times are supported but only if seconds and microseconds
//! are both negative.
std::string toStringDuration( const orca::Time& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
//! Negative durations are supported, but only if the very first 
//! character of the string is '-'.
int toTimeDuration( const std::string &, orca::Time& );

//! Returns string in human readable format:
//! MM/DD/YY HH:MM:SS.sss.
std::string toString( const orca::Time& );
//! Returns string in format HH:MM:SS.sss.
std::string toString( const orca::TimeOfDay& );
//! Returns string in format YYYY/MM/DD.
std::string toString( const orca::Date& );

//! Parses the value into a Time object assuming ':' delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsTimeDuration( const Ice::PropertiesPtr &, const ::std::string& key, orca::Time &value );
//! Returns the default value if key is not found or cannot be converted to a Time.
orca::Time getPropertyAsTimeDurationWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Time &d );

} // namespace

#endif
