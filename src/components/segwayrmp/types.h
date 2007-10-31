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

#include <orcarobotdriverutil/ihwdriver.h>

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

    std::string toString() const;
};

class Command
{
public:
    double vx;
    double w;

    std::string toString() const;
};

} // namespace

#endif
