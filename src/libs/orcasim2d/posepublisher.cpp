#include "posepublisher.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcasim2d {

void
PosePublisher::publish( const hydronavutil::Pose &pose )
{
    orca::Localise2dData orcaData;
    orcaData.timeStamp = orcaice::getNow();
            
    orca::Pose2dHypothesis hyp;
    hyp.mean = orcanavutil::convert( pose );
    hyp.cov.xx = 1.0e-5;
    hyp.cov.xy = 0;
    hyp.cov.xt = 0;
    hyp.cov.yy = 1.0e-5;
    hyp.cov.yt = 0;
    hyp.cov.tt = 1.0e-5;
    hyp.weight = 1.0;
    orcaData.hypotheses.push_back( hyp );
    localise2dImpl_->localSetAndSend( orcaData );
}


}

