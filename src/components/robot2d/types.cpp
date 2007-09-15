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

std::string 
Command::toString() const
{
    stringstream ss;
    ss << "[vx="<<vx<<"m/s, vy="<<vy<<"m/s, w="<<w*180.0/M_PI<<"deg/s]";
    return ss.str();
}

std::string 
Data::toString() const
{
    stringstream ss;
    ss << endl;
    ss << "  t="<<seconds<<":"<<useconds<<endl;
    ss << "  [x,y,o]=["<<x<<","<<y<<","<<o*180.0/M_PI<<"]"<<endl;
    ss << "  [vx,vy,w]=["<<vx<<","<<vy<<","<<w<<"]"<<endl;
    return ss.str();
}

}

