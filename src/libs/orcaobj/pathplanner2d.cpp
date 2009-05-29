/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <orcaifaceutil/pathplanner2d.h>
#include "pathplanner2d.h"
#include "pathfollower2d.h"

using namespace std;

namespace orcaobj
{

std::string toVerboseString( const orca::PathPlanner2dData& obj )
{
    std::stringstream s;

    s << obj.timeStamp.seconds << "s:" << obj.timeStamp.useconds << "us"
      << " PathPlanner2dDataPtr [" << obj.path.size() << " waypoints. "
      << "Result code: " << ifaceutil::toString(obj.result) << " ]:" << endl;
    s << "Result description: " << obj.resultDescription << endl;
    for ( unsigned int i=0; i < obj.path.size(); i++ )
    {
        s << "  " << i << ":" << orcaobj::toString(obj.path[i]) << endl;
    }

    return s.str();
}

std::string toVerboseString( const orca::PathPlanner2dTask& obj )
{
    std::stringstream s;

    s << obj.timeStamp.seconds << "s:" << obj.timeStamp.useconds << "us"
      << " PathPlanner2dTaskPtr [path contains "<< obj.coarsePath.size() << " waypoints]:" << endl;
    for ( unsigned int i=0; i < obj.coarsePath.size(); i++ )
    {
        s << "  " << i << ":" << orcaobj::toString(obj.coarsePath[i]) << endl;
    }

    return s.str();
}

} // namespace
