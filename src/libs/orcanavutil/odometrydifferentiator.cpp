#include "odometrydifferentiator.h"
#include <iostream>
#include <orcanavutil/orcanavutil.h>

using namespace std;

namespace orcanavutil {

void
OdometryDifferentiator::addOdom( const orca::Odometry2dData& odom, Offset &delta )
{
    if ( !prevOdomInitialised_ )
    {
        prevOdom_.pose.p.x = odom.pose.p.x;
        prevOdom_.pose.p.y = odom.pose.p.y;
        prevOdom_.pose.o = odom.pose.o;

        delta.x     = 0.0;
        delta.y     = 0.0;
        delta.theta = 0.0;

        prevOdomInitialised_ = true;
//         cout<<"TRACE(odometrydifferentiator.cpp): Initialising prevOdom_" << endl;
        return;
    }

    // Calculate the offset that this new odometry info represents
    orcanavutil::subtractInitialOffset( odom.pose.p.x,
                                        odom.pose.p.y,
                                        odom.pose.o,
                                        prevOdom_.pose.p.x,
                                        prevOdom_.pose.p.y,
                                        prevOdom_.pose.o,
                                        delta.x,
                                        delta.y,
                                        delta.theta );

    orcanavutil::normaliseAngle( delta.theta );

//     cout<<"TRACE(odometrydifferentiator.cpp): prevOdom: " << prevOdom_.pose << endl;
//     cout<<"TRACE(odometrydifferentiator.cpp):     odom: " << odom.pose << endl;
//     cout<<"TRACE(odometrydifferentiator.cpp): delta   : " << delta << endl;
    
    prevOdom_.pose.p.x = odom.pose.p.x;
    prevOdom_.pose.p.y = odom.pose.p.y;
    prevOdom_.pose.o   = odom.pose.o;
}

}
