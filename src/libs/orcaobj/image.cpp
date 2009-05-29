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

#include "image.h"
#include "datetime.h"
// #include <orcaifaceutil/image.h>

using namespace std;

namespace orcaobj
{

std::string toString( const orca::ImageDataPtr& obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " ImageData: ["<<obj->pixelData.size()<<" bytes]\n";
    return s.str();
}

std::string toString( const orca::ImageDescriptionPtr& obj )
{
    std::ostringstream s;
    s << "ImageDescription:\n" 
      << "Image height              " << obj->height << "pix\n"
      << "Image width               " << obj->width << "pix\n"
      << "Format                    " << obj->format << "\n";
    return s.str();
}

} // namespace
