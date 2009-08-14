/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "robot2d.h"

static const double Pi = 3.14159265358979323846264338327950288419717;

namespace hydrointerfaces {

std::string 
Robot2d::Data::toString() const
{
    std::stringstream ss;

    ss << "  seconds:  " << seconds  << std::endl;
    ss << "  useconds: " << useconds << std::endl;
    ss << "  x:        " << x        << std::endl;
    ss << "  y:        " << y        << std::endl;
    ss << "  yaw:      " << yaw      << std::endl;
    ss << "  vlong:    " << vlong    << std::endl;
    ss << "  vtrans:   " << vtransverse << std::endl;
    ss << "  dyaw:     " << dyaw     << std::endl;

    return ss.str(); 
}

std::string 
Robot2d::Command::toString() const
{
    std::stringstream ss;
    ss << "[ "<<vlong<<"m/s, "<<vtransverse<<"m/s, "<<dyaw*180.0/Pi<<"deg/s ]";

    return ss.str();
}

} // namespace
