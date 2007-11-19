/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_TYPES_H
#define ORCA2_SEGWAY_RMP_TYPES_H

#include <sstream>
#include <cmath>

namespace segwayrmp
{

class Data
{
public:

    int seconds;
    int useconds;
    double x;
    double y;
    double roll;
    double pitch;
    double yaw;
    double vx;
    double droll;
    double dpitch;
    double dyaw;
    double mainvolt;
    double uivolt;

    std::string toString() const
        {
            std::stringstream ss;

            ss << "  seconds:  " << seconds  << std::endl;
            ss << "  useconds: " << useconds << std::endl;
            ss << "  x:        " << x        << std::endl;
            ss << "  y:        " << y        << std::endl;
            ss << "  roll:     " << roll     << std::endl;
            ss << "  pitch:    " << pitch    << std::endl;
            ss << "  yaw:      " << yaw      << std::endl;
            ss << "  vx:       " << vx       << std::endl;
            ss << "  droll:    " << droll    << std::endl;
            ss << "  dpitch:   " << dpitch   << std::endl;
            ss << "  dyaw:     " << dyaw     << std::endl;
            ss << "  mainvolt: " << mainvolt << std::endl;
            ss << "  uivolt:   " << uivolt   << std::endl;

            return ss.str(); 
        }
};

class Command
{
public:
    double vx;
    double w;

    std::string toString() const
        {
            std::stringstream ss;
            ss << "[ "<<vx<<"m/s, "<<w*180.0/M_PI<<"deg/s ]";

            return ss.str();
        }
};

} // namespace

#endif
