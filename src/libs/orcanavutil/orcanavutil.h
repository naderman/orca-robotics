#ifndef ORCANAVUTIL_ORCANAVUTIL_H
#define ORCANAVUTIL_ORCANAVUTIL_H

#include <hydronavutil/hydronavutil.h>
#include <orca/localise2d.h>
#include <orca/odometry2d.h>

namespace orcanavutil {

    hydronavutil::Pose     convert( const orca::Frame2d &frame );
    hydronavutil::Cov3d    convert( const orca::Covariance2d &c );
    hydronavutil::Gaussian convert( const orca::Pose2dHypothesis &hyp );
    hydronavutil::Gmm      convert( const orca::Localise2dData &locData );
    hydronavutil::Pose     convertToPose( const orca::Odometry2dData &odom );
    hydronavutil::Velocity convertToVelocity( const orca::Odometry2dData &odom );

    //! Note: the timestamp has to be supplied separately since the Gmm doesn't have one.
    orca::Localise2dData   convert( const hydronavutil::Gmm &gmm, int seconds, int useconds );

}

#endif
