/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "localnavdriver.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace localnav {

std::ostream &operator<<( std::ostream &s, LocalNavParameters &n )
{
    s << "LocalNavParameters: " << endl;
    s << "\t maxSpeed                 : " << n.maxSpeed << endl
      << "\t maxTurnrate              : " << n.maxTurnrate*180.0/M_PI << "deg/s" << endl;
    return s;
}

std::ostream &operator<<( std::ostream &s, LocalNavDriver::DriverState state )
{
    switch (state)
    {
    case LocalNavDriver::STATE_GOAL_REACHED:
    {
        s << "GOAL_REACHED";
        break;
    }
    case LocalNavDriver::STATE_TURNING_AT_GOAL:
    {
        s << "TURNING_AT_GOAL";
        break;
    }
    case LocalNavDriver::STATE_ESCAPING:
    {
        s << "ESCAPING";
        break;
    }
    case LocalNavDriver::STATE_MOVING_TO_GOAL:
    {
        s << "MOVING_TO_GOAL";
        break;
    }
    default:
    {
        s << "Error: Unknown state";
        break;
    }        
    }
    return s;
}

}
