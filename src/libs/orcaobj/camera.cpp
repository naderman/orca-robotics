/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <gbxutilacfr/mathdefs.h>

#include "camera.h"

using namespace std;

namespace orcaobj
{

std::string 
toString( const orca::CameraDescriptionPtr& obj )
{
    std::ostringstream s;
    s << "Image height              " << obj->height << "pix" << endl;
    s << "Image width               " << obj->width << "pix" << endl;
    s << "Frame rate                " << obj->frameRate << "fps" << endl;
    s << "Format                    " << obj->format << endl;
    s << "offset.point.x            " << obj->offset.p.x << "m" << endl;
    s << "offset.point.y            " << obj->offset.p.y << "m" << endl;
    s << "offset.point.z            " << obj->offset.p.z << "m" << endl;
    s << "offset.orientation.roll   " << RAD2DEG(obj->offset.o.r) << "deg" << endl;
    s << "offset.orientation.pitch  " << RAD2DEG(obj->offset.o.p) << "deg" << endl;
    s << "offset.orientation.yaw    " << RAD2DEG(obj->offset.o.y) << "deg" << endl;

    return s.str();
}

} // namespace
