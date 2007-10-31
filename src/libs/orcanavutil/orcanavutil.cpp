#include "orcanavutil.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcanavutil {

    hydronavutil::Pose convert( const orca::Frame2d &frame )
    {
        return hydronavutil::Pose( frame.p.x, frame.p.y, frame.o );
    }

    hydronavutil::Cov3d convert( const orca::Covariance2d &c )
    {
        return hydronavutil::Cov3d( c.xx,
                                    c.xy,
                                    c.xt,
                                    c.yy,
                                    c.yt,
                                    c.tt );
    }

    hydronavutil::Gaussian convert( const orca::Pose2dHypothesis &hyp )
    {
        return hydronavutil::Gaussian( convert(hyp.mean), 
                                       convert(hyp.cov) );
    }

    hydronavutil::Gmm convert( const orca::Localise2dData &locData )
    {
        std::vector<hydronavutil::Gaussian> components;
        std::vector<double>                 weights;

        if ( locData.hypotheses.size() == 0 )
        {
            stringstream ss;
            ss << "orcanavutil::convert: input Localise2dData has zero hypotheses:"
               << endl << orcaice::toString( locData );
            throw hydroutil::Exception( ERROR_INFO, ss.str() );
        }

        for ( uint i=0; i < locData.hypotheses.size(); i++ )
        {
            components.push_back( convert( locData.hypotheses[i] ) );
            weights.push_back( locData.hypotheses[i].weight );
        }

        return hydronavutil::Gmm( components, weights );
    }

    hydronavutil::Pose convert( const orca::Odometry2dData &odom )
    {
        return hydronavutil::Pose( odom.pose.p.x, odom.pose.p.y, odom.pose.o );
    }
}

