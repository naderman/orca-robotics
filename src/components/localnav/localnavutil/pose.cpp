#include "pose.h"
#include <iostream>
#include <sstream>

// for mlHypothesis()
#include <orcaobj/miscutils.h>

using namespace std;

namespace localnav {

orcanavutil::Pose
getMLPose( const orca::Localise2dData &localiseData )
{
    const orca::Pose2dHypothesis &h = orcaice::mlHypothesis( localiseData );
    return orcanavutil::Pose( h.mean.p.x, h.mean.p.y, h.mean.o );
}

}
