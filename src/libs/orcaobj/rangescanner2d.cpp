/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <gbxutilacfr/mathdefs.h>
#include "rangescanner2d.h"
#include "datetime.h"

using namespace std;

namespace orcaobj
{

std::string 
toString( const orca::RangeScanner2dDescription& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " RangeScanner2dDescription: \n"
        << "minRange=" << obj.minRange << "m\n"
        << "maxRange=" << obj.maxRange << "m\n"
        << "fieldOfView=" << RAD2DEG(obj.fieldOfView) << "deg\n"
        << "startAngle=" << RAD2DEG(obj.startAngle) << "deg\n"
        << "numberOfSamples=" << obj.numberOfSamples << "\n"
        << "offset.point.x=" << obj.offset.p.x << "m\n"
        << "offset.point.y=" << obj.offset.p.y << "m\n"
        << "offset.point.z=" << obj.offset.p.z << "m\n"
        << "offset.orientation.roll=" << RAD2DEG(obj.offset.o.r) << "deg\n"
        << "offset.orientation.pitch=" << RAD2DEG(obj.offset.o.p) << "deg\n"
        << "offset.orientation.yaw=" << RAD2DEG(obj.offset.o.y) << "deg\n"
        << "size.length=" << obj.size.l << "m\n"
        << "size.width=" << obj.size.w << "m\n"
        << "size.height=" << obj.size.h << "m";

    return s.str();
}

} // namespace
