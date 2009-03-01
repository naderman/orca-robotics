/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PATHFILE_UTIL
#define ORCA_PATHFILE_UTIL

#include <orca/pathfollower2d.h>

namespace pathfileutil
{

//! Converts Waypoint2d to text which is easy to parse
std::string toLogString( const orca::Waypoint2d& );

//! Parses from ASCII log
void fromLogString( std::stringstream&, orca::Waypoint2d& );

//! Converts Path2d to text which is easy to parse
std::string toLogString( const orca::Path2d& );

}

#endif
