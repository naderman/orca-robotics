#include "scanutil.h"
#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

namespace hydroscanutil {

bool ScannerConfig::isSane() const
{
    if ( maxRange < 0 )
    {
        cout << "ERROR(scanutil.cpp): maxRange<0" << endl;
        return false;
    }
    if ( startAngle < -M_PI || startAngle > M_PI )
    {
        cout << "ERROR(scanutil.cpp): startAngle outside range [-pi,pi]" << endl;
        return false;
    }
    if ( angleIncrement < 0 )
    {
        cout << "ERROR(scanutil.cpp): angleIncrement<0" << endl;
        return false;
    }
    return true;
}

std::string toString( const ScannerConfig &c )
{
    stringstream ss;
    ss << "maxRange: " << c.maxRange << ", startAngle: " << c.startAngle*180.0/M_PI << "deg, angleIncrement: " << c.angleIncrement*180.0/M_PI << "deg";
    return ss.str();
}

}

