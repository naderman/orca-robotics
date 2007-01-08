/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "idriver.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace localnav {

std::ostream &operator<<( std::ostream &s, IDriver::DriverState state )
{
    switch (state)
    {
    case IDriver::STATE_GOAL_REACHED:
    {
        s << "GOAL_REACHED";
        break;
    }
    case IDriver::STATE_TURNING_AT_GOAL:
    {
        s << "TURNING_AT_GOAL";
        break;
    }
    case IDriver::STATE_ESCAPING:
    {
        s << "ESCAPING";
        break;
    }
    case IDriver::STATE_MOVING_TO_GOAL:
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
