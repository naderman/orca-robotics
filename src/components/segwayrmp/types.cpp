#include "types.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

namespace segwayrmp {

std::string
Data::toString() const 
{
    stringstream ss;

    ss << "  seconds:  " << seconds  << endl;
    ss << "  useconds: " << useconds << endl;
    ss << "  x:        " << x        << endl;
    ss << "  y:        " << y        << endl;
    ss << "  roll:     " << roll     << endl;
    ss << "  pitch:    " << pitch    << endl;
    ss << "  yaw:      " << yaw      << endl;
    ss << "  vx:       " << vx       << endl;
    ss << "  droll:    " << droll    << endl;
    ss << "  dpitch:   " << dpitch   << endl;
    ss << "  dyaw:     " << dyaw     << endl;
    ss << "  mainvolt: " << mainvolt << endl;
    ss << "  uivolt:   " << uivolt   << endl;

    return ss.str(); 
}

std::string
Command::toString() const
{
    stringstream ss;
    ss << "[ "<<vx<<"m/s, "<<w*180.0/M_PI<<"deg/s ]";

    return ss.str();
}

}

