#include "pose.h"
#include <iostream>
#include <sstream>

// for mlHypothesis()
#include <orcaobj/miscutils.h>

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

orcanavutil::Pose
getMLPose( const orca::Localise2dData &localiseData )
{
    const orca::Pose2dHypothesis &h = orcaice::mlHypothesis( localiseData );
    return orcanavutil::Pose( h.mean.p.x, h.mean.p.y, h.mean.o );
}

}
