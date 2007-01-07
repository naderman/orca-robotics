/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

/*
 A bunch of utilities for:
  - rotating points (as vectors).
  - converting points between frames.

  for further reading:

  http://kwon3d.com/theory/transform/rot.html
  http://mathworld.wolfram.com/EulerAngles.html

*/

#ifndef ORCAOBJ_FRAMEUTILS_H
#define ORCAOBJ_FRAMEUTILS_H

#include <orca/bros1.h>

namespace orcaice
{
    // Rotate a 2d point by an angle (about Z-axis)
    orca::CartesianPoint2d rotate2d(const orca::CartesianPoint2d &point, const double &angle);

    // Rotate a 3d point by an angle about the X axis
    orca::CartesianPoint rotate3dX(const orca::CartesianPoint &point, const double &angle);

    // Rotate a 3d point by an angle about the Y axis
    orca::CartesianPoint rotate3dY(const orca::CartesianPoint &point, const double &angle);

    // Rotate a 3d point by an angle about the Z axis
    orca::CartesianPoint rotate3dZ(const orca::CartesianPoint &point, const double &angle);

    // Convert a global point to another, relative to a frame
    orca::CartesianPoint2d convertToFrame2d(const orca::Frame2d &frame, const orca::CartesianPoint2d &point);

    // Convert a global point to another, relative to a frame
    orca::CartesianPoint convertToFrame3d(const orca::Frame3d &frame, const orca::CartesianPoint &point);

    // Convert a point relative to a frame back to the global frame
    orca::CartesianPoint2d convertFromFrame2d(const orca::Frame2d &frame, const orca::CartesianPoint2d &point);

    // Convert a point relative to a frame back to the global frame
    orca::CartesianPoint convertFromFrame3d(const orca::Frame3d &frame, const orca::CartesianPoint &point);

}

#endif
