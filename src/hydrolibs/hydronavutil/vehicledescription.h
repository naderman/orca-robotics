#ifndef HYDRONAVUTIL_VEHICLEDESCRIPTION_H
#define HYDRONAVUTIL_VEHICLEDESCRIPTION_H

#include <hydrobros1/types.h>

namespace hydronavutil {

struct DiffDriveControlDescription {
    double maxForwardSpeed;
    double maxReverseSpeed; // (this is a positive number)
    double maxTurnrate;
    double maxLateralAcceleration;
    double maxForwardAcceleration;
    double maxReverseAcceleration;
    double maxRotationalAcceleration;
};

struct CylindricalGeometryDescription {
    double              radius;
    double              height;
    hydrobros1::Frame3d platformToGeometryTransform;
};

}

#endif
