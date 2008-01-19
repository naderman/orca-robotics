/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>

#include "frameutils.h"

using namespace std;
using namespace orca;


namespace orcaice {

    // Rotate a 2d point by an angle (about Z-axis)
    CartesianPoint2d rotate2d(const CartesianPoint2d &point, const double &angle)
    {
        CartesianPoint2d p;
        p.x=point.x*cos(angle)-point.y*sin(angle);
        p.y=point.x*sin(angle)+point.y*cos(angle);

        return p;
    }

    // Rotate a 3d point by an angle about the X axis
    CartesianPoint rotate3dX(const CartesianPoint &point, const double &angle)
    {
        CartesianPoint p;
        p.x=point.x;
        p.y=point.y*cos(angle)-point.z*sin(angle);
        p.z=point.y*sin(angle)+point.z*cos(angle);
        return p;
    }

    // Rotate a 3d point by an angle about the Y axis
    CartesianPoint rotate3dY(const CartesianPoint &point, const double &angle)
    {
        CartesianPoint p;
        p.x=point.x*cos(angle)+point.z*sin(angle);
        p.y=point.y;
        p.z=-point.x*sin(angle)+point.z*cos(angle);

        return p;
    }

    // Rotate a 3d point by an angle about the Z axis
    CartesianPoint rotate3dZ(const CartesianPoint &point, const double &angle)
    {
        CartesianPoint p;
        p.x=point.x*cos(angle)-point.y*sin(angle);
        p.y=point.x*sin(angle)+point.y*cos(angle);
        p.z=point.z;

        return p;
    }

    // Note: when rotating frames we use negative angles

    // Convert a point to another relative to a frame
    CartesianPoint2d convertToFrame2d(const Frame2d &frame, const CartesianPoint2d &point)
    {
        CartesianPoint2d p;
        // translate
        p.x=point.x-frame.p.x;
        p.y=point.y-frame.p.y;
        // rotate
        p=rotate2d(p,-frame.o);
        return p;
    }

    // Convert a point to another relative to a frame
    CartesianPoint convertToFrame3d(const Frame3d &frame, const CartesianPoint &point)
    {
        CartesianPoint p;
        // translate
        p.x=point.x-frame.p.x;
        p.y=point.y-frame.p.y;
        p.z=point.z-frame.p.z;
        // rotate roll pitch yaw
        p=rotate3dX(p,-frame.o.r);
        p=rotate3dY(p,-frame.o.p);
        p=rotate3dZ(p,-frame.o.y);
        return p;
    }

    // Note the positive angles to revert to the original frame

    // Convert a point relative to a frame back to the global frame
    CartesianPoint2d convertFromFrame2d(const Frame2d &frame, const CartesianPoint2d &point)
    {
        CartesianPoint2d p;
        // first rotate back to global frame
        p=rotate2d(point,frame.o);
        // translate
        p.x+=frame.p.x;
        p.y+=frame.p.y;
        return p;

    }

    // Convert a point relative to a frame back to the global frame
    CartesianPoint convertFromFrame3d(const Frame3d &frame, const CartesianPoint &point)
    {
        CartesianPoint p;
        // rotate yaw pitch roll
        p=rotate3dZ(point,frame.o.y);
        p=rotate3dY(p,frame.o.p);
        p=rotate3dX(p,frame.o.r);
        // translate
        p.x+=frame.p.x;
        p.y+=frame.p.y;
        p.z+=frame.p.z;

        return p;
    }

    orca::Frame3d 
    zeroFrame3d()
    {
        orca::Frame3d f;
        f.p.x = 0;
        f.p.y = 0;
        f.p.z = 0;
        f.o.r = 0;
        f.o.p = 0;
        f.o.y = 0;
        return f;
    }

}

