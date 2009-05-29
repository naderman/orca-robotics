/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_PATH_PLANNER_H
#define ORCAOBJ_PATH_PLANNER_H

#include <orca/pathplanner2d.h>

namespace orcaobj
{

//! Prints out the path and result code
std::string toVerboseString( const orca::PathPlanner2dData& );
//! Prints out the coarse path given as a task
std::string toVerboseString( const orca::PathPlanner2dTask& );

} // namespace

#endif
