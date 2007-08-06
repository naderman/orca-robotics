/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_TYPES_H
#define ORCA2_ROBOT2D_TYPES_H

#include <string>

namespace robot2d
{

struct Data
{
    int seconds;
    int useconds;
    double x;
    double y;
    double o;
    double vx;
    double vy;
    double w;
};

struct Command
{
    double vx;
    double vy;
    double w;
};

struct Stats
{
    double distanceTravelled;
};

std::string toString( const Command &c );

} // namespace

#endif
