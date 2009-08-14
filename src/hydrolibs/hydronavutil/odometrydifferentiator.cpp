#include "odometrydifferentiator.h"
#include <iostream>
#include <hydronavutil/hydronavutil.h>

using namespace std;

namespace hydronavutil {

OdometryDifferentiator::OdometryDifferentiator( const hydronavutil::Pose &initPose )
    : prevOdomInitialised_(true)
{
    prevX_     = initPose.x();
    prevY_     = initPose.y();
    prevTheta_ = initPose.theta();
}

hydronavutil::Pose
OdometryDifferentiator::calcDelta( double x,
                                   double y,
                                   double theta )
{
    if ( !prevOdomInitialised_ )
    {
        prevX_     = x;
        prevY_     = y;
        prevTheta_ = theta;

        prevOdomInitialised_ = true;

        return Pose( 0.0, 0.0, 0.0 );
    }

    // Calculate the offset that this new odometry info represents
    Pose delta;
    hydronavutil::subtractInitialOffset( x,
                                         y,
                                         theta,
                                         prevX_,
                                         prevY_,
                                         prevTheta_,
                                         delta.x(),
                                         delta.y(),
                                         delta.theta() );

    hydronavutil::normaliseAngle( delta.theta() );

    prevX_     = x;
    prevY_     = y;
    prevTheta_ = theta;

    return delta;
}

}
