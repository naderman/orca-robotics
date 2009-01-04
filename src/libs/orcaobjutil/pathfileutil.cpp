#include "pathfileutil.h"

using namespace std;

namespace pathfileutil
{
    
std::string 
toLogString( const orca::Waypoint2d& obj )
{
    std::ostringstream s;
    s << obj.target.p.x << " " << obj.target.p.y << " " << obj.target.o << " " 
      << obj.timeTarget.seconds << " " << obj.timeTarget.useconds << " " 
      << obj.distanceTolerance << " " 
      << obj.headingTolerance << " "
      << obj.maxApproachSpeed << " "
      << obj.maxApproachTurnrate
      << endl;
    return s.str();
}

void fromLogString( std::stringstream& stream, orca::Waypoint2d& obj )
{
    stream >> obj.target.p.x >> obj.target.p.y >> obj.target.o;
    stream >> obj.timeTarget.seconds >> obj.timeTarget.useconds;
    stream >> obj.distanceTolerance;
    stream >> obj.headingTolerance;
    stream >> obj.maxApproachSpeed;
    stream >> obj.maxApproachTurnrate;
}

std::string 
toLogString( const orca::Path2d& obj )
{
    std::ostringstream s;
    for (unsigned int i=0; i<obj.size(); i++)
    {
        s << toLogString( obj[i] ); 
    }
    return s.str();
}

}

