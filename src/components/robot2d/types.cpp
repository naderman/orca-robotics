/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "types.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

namespace robot2d {

std::string toString( const Command &c )
{
    stringstream ss;
    ss << "[vx="<<c.vx<<"m/s, vy="<<c.vy<<"m/s, w="<<c.w*180.0/M_PI<<"deg/s]";
    return ss.str();
}

}

