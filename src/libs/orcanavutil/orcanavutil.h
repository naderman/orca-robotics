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

    hydronavutil::Pose     convert( const orca::Odometry2dData &odom );

}

#endif
