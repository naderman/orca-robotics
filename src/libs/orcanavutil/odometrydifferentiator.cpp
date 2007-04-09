#include "odometrydifferentiator.h"
#include <iostream>
#include <orcanavutil/orcanavutil.h>

using namespace std;

namespace orcanavutil {

void
OdometryDifferentiator::addOdom( double x,
                                 double y,
                                 double theta,
                                 Offset &delta )
{
    if ( !prevOdomInitialised_ )
    {
        prevX_     = x;
        prevY_     = y;
        prevTheta_ = theta;

        delta.x     = 0.0;
        delta.y     = 0.0;
        delta.theta = 0.0;

        prevOdomInitialised_ = true;
//         cout<<"TRACE(odometrydifferentiator.cpp): Initialising prevOdom_" << endl;
        return;
    }

    // Calculate the offset that this new odometry info represents
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
}

}
