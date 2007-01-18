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

}

#endif
