/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_PATHFOLLOWER2D_H
#define ORCAOBJ_PATHFOLLOWER2D_H

#include <orca/pathfollower2d.h>

namespace orcaobj
{

//!
//! If there's something wrong with the path object, returns false
//! and fills out the 'reason' field.
//! If it's OK, returns true.
//!
bool isSane( const orca::Path2d& path, std::string& reason );

//!
//! If there's something wrong with the data structure, returns false
//! and fills out the 'reason' field.
//! If it's OK, returns true.
//!
bool isSane( const orca::PathFollower2dData& data, std::string& reason );

bool isPathSketchy( const orca::Path2d& path, std::string &sketchyReason );

//! Converts to string
std::string toString( const orca::Waypoint2d& );
//! Converts to string
std::string toString( const orca::PathFollower2dData& );

//! Prints out each waypoint
std::string toVerboseString( const orca::PathFollower2dData& );

} // namespace

#endif
