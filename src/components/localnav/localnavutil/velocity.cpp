#include "velocity.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

namespace localnav {

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
