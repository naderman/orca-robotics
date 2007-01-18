#include "pose.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace localnav {

std::string
Pose::toString() const
{
    stringstream ss;
    ss << "[" << x() << ", " << y() << ", " << theta()*180.0/M_PI << "deg]";
    return ss.str();
}

std::ostream &operator<<( std::ostream &s, const Pose &p )
{
    return s << p.toString();
}

}
