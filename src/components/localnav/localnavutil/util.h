#ifndef ORCA_LOCALNAVUTIL_UTIL_H
#define ORCA_LOCALNAVUTIL_UTIL_H

namespace localnav {

    // Linearly interpolate between min and max steer angle according to the speed.
    // Used for bicycle model robots
    inline double maxSteerAngleAtCurrentSpeed( double maxSteerAngle0ms,
                                               double maxSteerAngleAtMaxSpeed,
                                               double maxSpeed,
                                               double currentSpeed )
    {
        return maxSteerAngle0ms - (maxSteerAngle0ms-maxSteerAngleAtMaxSpeed)*(currentSpeed/maxSpeed);
    }

    // certainly greater than: when numerical issues are involved
    // certainly: a > b
    inline bool certainGT( double a, double b, double eps=1e-4 )
    {
        return ( (a-b) > eps );
    }

    // certainly less than: when numerical issues are involved
    // certainly: a < b
    inline bool certainLT( double a, double b, double eps=1e-4 )
    {
        return ( (a-b) < -eps );
    }

}

#endif
