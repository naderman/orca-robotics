#include "odometrydifferentiator.h"
#include <iostream>
#include <orcanavutil/orcanavutil.h>

using namespace std;

namespace orcanavutil {

orcanavutil::Offset
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

        return Offset( 0.0, 0.0, 0.0 );
    }

    // Calculate the offset that this new odometry info represents
    Offset delta;
    orcanavutil::subtractInitialOffset( x,
                                        y,
                                        theta,
                                        prevX_,
                                        prevY_,
                                        prevTheta_,
                                        delta.x,
                                        delta.y,
                                        delta.theta );

    orcanavutil::normaliseAngle( delta.theta );

    prevX_     = x;
    prevY_     = y;
    prevTheta_ = theta;

    return delta;
}

}
