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
#include <gbxutilacfr/mathdefs.h>

// trying to solve a problem in win, round() is not found
#ifdef WIN32
#   include <math.h>
inline double round( double d )
{
	return floor( d + 0.5 );
}
#else
#   include <cmath>
#endif

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#include "velocitycontrol2d.h"

using namespace std;

namespace orcaobj
{

std::string toString( const orca::VelocityControl2dData& obj )
{
    std::ostringstream s;
    s <<   "VelocityControl2dData (vx,vy,w(deg/s)) : ("
            << obj.motion.v.x << ", "
            << obj.motion.v.y << ", "
            << RAD2DEG(obj.motion.w) << ")";

    return s.str();
}

} // namespace
