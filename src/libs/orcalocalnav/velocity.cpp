/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "velocity.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <orca/velocitycontrol2d.h>


using namespace std;

namespace orcalocalnav {

// std::string 
// Velocity::toString() const
// {
//     stringstream ss;
//     ss << "["<<lin()<<"m/s, "<<
// }

std::ostream &operator<<( std::ostream &s, const Velocity &v )
{
    return s <<"["<<v.lin()<<"m/s, "<<v.rot()*180.0/M_PI<<"deg/s]";
}

std::ostream &operator<<( std::ostream &s, const Acceleration &v )
{
    return s <<"["<<v.lin()<<"m/s/s, "<<v.rot()*180.0/M_PI<<"deg/s/s]";
}

}
