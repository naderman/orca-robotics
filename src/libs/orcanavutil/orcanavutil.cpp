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

        try {
            return hydronavutil::Gmm( components, weights );
        }
        catch ( const hydronavutil::Exception &e )
        {
            stringstream ss;
            ss << "While creating Gmm from orca::Localise2dData: " << orcaice::toString(locData)
               << ": " << e.what();
            throw hydronavutil::Exception( ERROR_INFO, ss.str() );
        }
    }

    hydronavutil::Pose convert( const orca::Odometry2dData &odom )
    {
        return hydronavutil::Pose( odom.pose.p.x, odom.pose.p.y, odom.pose.o );
    }

    orca::Localise2dData   convert( const hydronavutil::Gmm &gmm, int seconds, int useconds )
    {
        orca::Localise2dData l;
        l.timeStamp.seconds  = seconds;
        l.timeStamp.useconds = useconds;

        l.hypotheses.resize( gmm.size() );

        for ( uint i=0; i < gmm.size(); i++ )
        {
            orca::Pose2dHypothesis h;

            h.mean.p.x = gmm.components(i).mean().x();
            h.mean.p.y = gmm.components(i).mean().y();
            h.mean.o   = gmm.components(i).mean().theta();

            h.cov.xx = gmm.components(i).cov().xx();
            h.cov.xy = gmm.components(i).cov().xy();
            h.cov.xt = gmm.components(i).cov().xt();
            h.cov.yy = gmm.components(i).cov().yy();
            h.cov.yt = gmm.components(i).cov().yt();
            h.cov.tt = gmm.components(i).cov().tt();

            h.weight = gmm.weights(i);

            l.hypotheses[i] = h;
        }
        return l;
    }
}

