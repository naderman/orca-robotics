/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "velocity.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <hydroportability/windows.h>

using namespace std;

namespace hydronavutil {

std::ostream &operator<<( std::ostream &s, const Velocity &v )
{
    return s <<"["<<v.lin()<<"m/s, "<<v.rot()*180.0/M_PI<<"deg/s]";
}

std::ostream &operator<<( std::ostream &s, const Acceleration &v )
{
    return s <<"["<<v.lin()<<"m/s/s, "<<v.rot()*180.0/M_PI<<"deg/s/s]";
}

}
