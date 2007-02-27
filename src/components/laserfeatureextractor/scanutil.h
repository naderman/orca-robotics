#ifndef SCANUTIL_H
#define SCANUTIL_H

#include <orca/rangescanner2d.h>
#include <orcaobj/miscutils.h>

namespace laserfeatures {

inline double laserScanBearing( const orca::RangeScanner2dDataPtr &scan, const int i )
{
    double angleIncrement = orcaice::calcAngleIncrement( scan->fieldOfView, scan->ranges.size() );
    return (scan->startAngle + angleIncrement*i);
}

}

#endif
