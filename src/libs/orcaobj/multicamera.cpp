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
#include <sstream>
#include <gbxutilacfr/mathdefs.h>
#include "multicamera.h"

using namespace std;

namespace orcaobj
{

std::string toString( const orca::MultiCameraDataPtr& obj )
{
    std::ostringstream s;
    
    for( unsigned int i = 0; i < obj->cameraDataVector.size(); ++i )
    {
        s << " CameraData " << i+1 << " : ["<<obj->cameraDataVector[i]->pixelData.size()<<" bytes]\n";
    }

    return s.str();
}

std::string toString( const orca::MultiCameraDescriptionPtr& obj )
{
    std::ostringstream s;
    for( unsigned int i = 0; i < obj->descriptions.size(); ++i )
    {
        s << "Camera " << i << " :" << endl;
        s << "\tImage height              " << obj->descriptions[i]->height << "pix" << endl;
        s << "\tImage width               " << obj->descriptions[i]->width << "pix" << endl;
        s << "\tFrame rate                " << obj->descriptions[i]->frameRate << "fps" << endl;
        s << "\tFormat                    " << obj->descriptions[i]->format << endl;
        s << "\toffset.point.x            " << obj->descriptions[i]->offset.p.x << "m" << endl;
        s << "\toffset.point.y            " << obj->descriptions[i]->offset.p.y << "m" << endl;
        s << "\toffset.point.z            " << obj->descriptions[i]->offset.p.z << "m" << endl;
        s << "\toffset.orientation.roll   " << RAD2DEG(obj->descriptions[i]->offset.o.r) << "deg" << endl;
        s << "\toffset.orientation.pitch  " << RAD2DEG(obj->descriptions[i]->offset.o.p) << "deg" << endl;
        s << "\toffset.orientation.yaw    " << RAD2DEG(obj->descriptions[i]->offset.o.y) << "deg" << endl;
    }

    return s.str();
}

} // namespace
