#include "pathfileutil.h"
#include <orcaice/timeutils.h>

using namespace std;

namespace pathfileutil
{
    
std::string 
toLogString( const orca::Waypoint2d& obj )
{
    //
    // AlexB: Note weird handling of timeTarget to maintain backwards-compatibility with format of old log-files!
    //
    std::ostringstream s;
    orca::Time timeTarget = orcaice::toOrcaTime( obj.timeTarget );
    s << obj.target.p.x << " " << obj.target.p.y << " " << obj.target.o << " " 
      << timeTarget.seconds << " " << timeTarget.useconds << " " 
      << obj.distanceTolerance << " " 
      << obj.headingTolerance << " "
      << obj.maxApproachSpeed << " "
      << obj.maxApproachTurnrate
      << endl;
    return s.str();
}

void fromLogString( std::stringstream& stream, orca::Waypoint2d& obj )
{
    //
    // AlexB: Note weird handling of timeTarget to maintain backwards-compatibility with format of old log-files!
    //
    stream >> obj.target.p.x >> obj.target.p.y >> obj.target.o;
    orca::Time timeTarget;
    stream >> timeTarget.seconds >> timeTarget.useconds;
    stream >> obj.distanceTolerance;
    stream >> obj.headingTolerance;
    stream >> obj.maxApproachSpeed;
    stream >> obj.maxApproachTurnrate;
    obj.timeTarget = orcaice::timeAsDouble( timeTarget );
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

