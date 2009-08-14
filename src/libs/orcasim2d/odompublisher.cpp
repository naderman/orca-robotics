#include "odompublisher.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcasim2d {

void
OdomPublisher::publish( const hydronavutil::Pose     &pose,
                        const hydronavutil::Velocity &velocity )
{
    orca::Odometry2dData orcaData;
    orcaData.timeStamp = orcaice::getNow();
            
    orcaData.pose = orcanavutil::convert( pose );
    orcaData.motion.v.x = velocity.lin();
    orcaData.motion.v.y = 0.0;
    orcaData.motion.w = velocity.rot();
    odometry2dImpl_->localSetAndSend( orcaData );
}


}

