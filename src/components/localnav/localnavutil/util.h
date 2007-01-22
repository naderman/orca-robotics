#ifndef ORCA_LOCALNAVUTIL_UTIL_H
#define ORCA_LOCALNAVUTIL_UTIL_H

namespace localnav {

    inline double maxTurnrate( double maxTurnrate0ms,
                               double maxTurnrateAtMaxSpeed,
                               double maxSpeed,
                               double speed )
    {
        return maxTurnrate0ms - (maxTurnrate0ms-maxTurnrateAtMaxSpeed)*(speed/maxSpeed);
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
