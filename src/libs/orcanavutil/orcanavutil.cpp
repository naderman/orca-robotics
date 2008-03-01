#include "orcanavutil.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

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
               << endl << orcaobj::toString( locData );
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
            ss << "While creating Gmm from orca::Localise2dData: " << orcaobj::toString(locData)
               << ": " << e.what();
            throw hydronavutil::Exception( ERROR_INFO, ss.str() );
        }
    }

    hydronavutil::Pose convertToPose( const orca::Odometry2dData &odom )
    {
        return hydronavutil::Pose( odom.pose.p.x, odom.pose.p.y, odom.pose.o );
    }

    hydronavutil::Velocity convertToVelocity( const orca::Odometry2dData &odom )
    {
        return hydronavutil::Velocity( odom.motion.v.x, odom.motion.w );
    }

    orca::Pose2dHypothesis convert( const hydronavutil::Gaussian &g, double weight )
    {
        orca::Pose2dHypothesis h;
        
        h.mean.p.x = g.mean().x();
        h.mean.p.y = g.mean().y();
        h.mean.o   = g.mean().theta();
        
        h.cov.xx = g.cov().xx();
        h.cov.xy = g.cov().xy();
        h.cov.xt = g.cov().xt();
        h.cov.yy = g.cov().yy();
        h.cov.yt = g.cov().yt();
        h.cov.tt = g.cov().tt();        

        h.weight = weight;

        return h;
    }

    orca::Localise2dData   convert( const hydronavutil::Gmm &gmm, int seconds, int useconds )
    {
        orca::Localise2dData l;
        l.timeStamp.seconds  = seconds;
        l.timeStamp.useconds = useconds;

        l.hypotheses.resize( gmm.size() );

        for ( uint i=0; i < gmm.size(); i++ )
        {
            l.hypotheses[i] = convert( gmm.components(i), gmm.weights(i) );
        }
        return l;
    }


    orca::Localise2dData convertUpToNHypotheses( const hydronavutil::Gmm &gmm,
                                                 int maxHypotheses,
                                                 int seconds,
                                                 int useconds )
    {
        if ( maxHypotheses < 0 || (int)(gmm.size()) <= maxHypotheses )
        {
            // convert the lot
            return convert( gmm, seconds, useconds );
        }

        orca::Localise2dData l;
        l.timeStamp.seconds  = seconds;
        l.timeStamp.useconds = useconds;

        // Make a copy coz we'er gonna modify it.
        hydronavutil::Gmm gmmCopy = gmm;
        while ( (int)(l.hypotheses.size()) < maxHypotheses )
        {
            int mlI = gmmCopy.mlComponentI();
            l.hypotheses.push_back( convert( gmmCopy.components(mlI), gmmCopy.weights(mlI) ) );
            gmmCopy.weights(mlI) = 0;
        }
        return l;
    }
}

